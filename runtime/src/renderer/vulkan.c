#include <string.h>
#include <vulkan/vulkan_core.h>

#include "vulkan/vulkan.h"

#include "oge/core/memory.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/assertion.h"
#include "oge/renderer/vulkan.h"
#include "oge/renderer/renderer.h"
#include "oge/containers/darray.h"

// State
// TODO: write custom allocators for vulkan
struct {
  VkInstance instance;
  VkAllocationCallbacks *pAllocationCallbacks;
  #ifdef OGE_DEBUG
  VkDebugUtilsMessengerEXT debugMessenger;
  #endif
} s_vulkanState;

/************************************************
 *                    debug                     *
 ************************************************/
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData) {

  switch(messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      OGE_TRACE("Vulkan message:\n%s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      OGE_INFO("Vulkan message:\n%s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      OGE_WARN("Vulkan message:\n%s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      OGE_ERROR("Vulkan message:\n%s", pCallbackData->pMessage);
      break;
    default:
      OGE_TRACE("Vulkan message:\n%s", pCallbackData->pMessage);
  }

  return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(
  VkInstance instance,
  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
  const VkAllocationCallbacks* pAllocator,
  VkDebugUtilsMessengerEXT* pDebugMessenger) {

  PFN_vkCreateDebugUtilsMessengerEXT func =
    (PFN_vkCreateDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

  if (func) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }

  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroyDebugUtilsMessengerEXT(
  VkInstance instance,
  VkDebugUtilsMessengerEXT debugMessenger,
  const VkAllocationCallbacks* pAllocator) {

   PFN_vkDestroyDebugUtilsMessengerEXT func =
    (PFN_vkDestroyDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

  if (func) {
    func(instance, debugMessenger, pAllocator);
  }
}

void createDebugMessenger() {
  OGE_TRACE("Creating Vulkan debug messenger.");

  VkDebugUtilsMessengerCreateInfoEXT createInfo = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    .messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = debugCallback,
    .pUserData = 0,
  };

  VkResult result = createDebugUtilsMessengerEXT(
    s_vulkanState.instance, &createInfo,
    s_vulkanState.pAllocationCallbacks, &s_vulkanState.debugMessenger);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan debug messenger.");
  }
}

/************************************************
 *             validation functions             *
 ************************************************/
b8 checkValidationLayerSupport(const char *pLayerName) {
  OGE_TRACE("Checking vulkan validation layer support.");
  u32 layerCount;
  VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, NULL);

  VkLayerProperties *pLayerProperties =
    ogeAllocate(layerCount * sizeof(VkLayerProperties), OGE_MEMORY_TAG_ARRAY);
  vkEnumerateInstanceLayerProperties(&layerCount, pLayerProperties);

  for (u32 i = 0; i < layerCount; ++i) {
    if (strcmp(pLayerName, pLayerProperties[i].layerName) == 0) {
      ogeDeallocate(pLayerProperties);
      OGE_TRACE("Vulkan validation layer is supported.");
      return OGE_TRUE;
    }
  }

  ogeDeallocate(pLayerProperties);
  OGE_ERROR("Vulkan validation layer isn't supported.");
  return OGE_FALSE;
}

/************************************************
 *                initialization                *
 ************************************************/
b8 createInstance(OgeRendererInitInfo *pInitInfo) {
  VkApplicationInfo applicationInfo = {
    .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext              = 0,
    .apiVersion         = VK_API_VERSION_1_3,
    .pApplicationName   = pInitInfo->pApplicationName,
    .applicationVersion = pInitInfo->applicationVersion,
    .pEngineName        = "Osfabias Game Engine",
    .engineVersion =
      VK_MAKE_VERSION(OGE_VERSION_MAJOR, OGE_VERSION_MINOR,
                      OGE_VERSION_PATCH),
  };

  VkInstanceCreateInfo info = {
    .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext                   = 0,
    .flags                   = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
    .pApplicationInfo        = &applicationInfo,
  };

  const char **ppExtensionNames = ogeDArrayAllocate(4, sizeof(char*));

  #ifdef OGE_PLATFORM_APPLE
  const char *pPortabilityExtensionName = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
  ogeDArrayAppend(ppExtensionNames, &pPortabilityExtensionName);
  #endif

  #ifdef OGE_DEBUG
  const char *pValidationLayerName = "VK_LAYER_KHRONOS_validation";

  if (checkValidationLayerSupport(pValidationLayerName)) {
    info.enabledLayerCount   = 1;
    info.ppEnabledLayerNames = &pValidationLayerName;

    const char *pDebugExtensionName = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    ogeDArrayAppend(ppExtensionNames, &pDebugExtensionName);
  }
  else {
    info.enabledLayerCount   = 0;
    info.ppEnabledLayerNames = 0;
  }
  #endif

  info.enabledExtensionCount   = ogeDArrayLength(ppExtensionNames);
  info.ppEnabledExtensionNames = ppExtensionNames;

  VkResult result = vkCreateInstance(
    &info, s_vulkanState.pAllocationCallbacks, &s_vulkanState.instance);

  ogeDArrayDeallocate(ppExtensionNames);

  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan instance: %d.", result);
    return OGE_FALSE;
  }

  OGE_TRACE("Vulkan instance created.");
  return OGE_TRUE;
}

b8 init(OgeRendererInitInfo *pInitInfo) {
  OGE_TRACE("Initializing Vulkan renderer.");

  if (!createInstance(pInitInfo)) { return OGE_FALSE; }

  #ifdef OGE_DEBUG
  createDebugMessenger();
  #endif

  OGE_TRACE("Vulkan renderer initialized.");
  return OGE_TRUE;
}

/************************************************
 *                termination                   *
 ************************************************/
void terminate() {
  OGE_TRACE("Terminating Vulkan renderer.");
  destroyDebugUtilsMessengerEXT(s_vulkanState.instance, s_vulkanState.debugMessenger,
                                s_vulkanState.pAllocationCallbacks);
  OGE_TRACE("Vulkan debug messender destroyed.");
  vkDestroyInstance(s_vulkanState.instance, 0);
  OGE_TRACE("Vulkan instance destroyed.");
  OGE_TRACE("Vulkan renderer terminated.");
}

void ogeVulkanCreate(OgeRendererState *pState) {
  pState->pInitFunction = init;
  pState->pTerminateFunction = terminate;
}
