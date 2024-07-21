#include <string.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "oge/core/memory.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/assertion.h"
#include "oge/containers/darray.h"
#include "oge/renderer/debug.h"
#include "oge/renderer/querries.h"
#include "oge/renderer/renderer.h"
#include "oge/renderer/renderer-types.h"

// TODO: write custom allocators for vulkan
struct {
  b8 initialized;

  VkInstance instance;

  VkSurfaceKHR surface;

  VkPhysicalDevice physicalDevice;
  OgeQueueFamilyIndicies queueFamilyIndicies;

  VkAllocationCallbacks *pAllocator;
  #ifdef OGE_DEBUG
  VkDebugUtilsMessengerEXT debugMessenger;
  #endif
} s_rendererState = {
  .initialized = OGE_FALSE,

  .pAllocator = 0, // temporary, while custom allocator isn't written
};

#ifdef OGE_DEBUG
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
    .pfnUserCallback = ogeVulkanDebugCallback,
    .pUserData = 0,
  };

  const VkResult result = vkCreateDebugUtilsMessengerEXT(
    s_rendererState.instance, &createInfo,
    s_rendererState.pAllocator, &s_rendererState.debugMessenger);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan debug messenger.");
    return;
  }

  OGE_TRACE("Vulkan debug messenger created.");
}

b8 isValidationLayerSupported(const char *pLayerName) {
  OGE_TRACE("Checking Vulkan validation layer support.");
  u32 layerCount;
  const VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, NULL);
  VkLayerProperties pLayerProperties[layerCount];
  vkEnumerateInstanceLayerProperties(&layerCount, pLayerProperties);

  for (u32 i = 0; i < layerCount; ++i) {
    if (strcmp(pLayerName, pLayerProperties[i].layerName) == 0) {
      OGE_INFO("Vulkan validation layer is enabled.");
      return OGE_TRUE;
    }
  }

  OGE_ERROR("Vulkan validation layer isn't supported.");
  return OGE_FALSE;
}
#endif

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
    .flags                   = 0,
    .pApplicationInfo        = &applicationInfo,
  };

  u16 extensionCount;
  ogePlatformGetRequiredVkExtensions(&extensionCount);
  const char **ppExtensions =
    ogeDArrayAllocate(extensionCount, sizeof(char*));
  ppExtensions =
    ogeDArrayExtend(ppExtensions,
                    ogePlatformGetRequiredVkExtensions(0),
                    extensionCount);

  #ifdef OGE_PLATFORM_APPLE
  const char *pPortabilityExtensionName = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
  ppExtensions = ogeDArrayAppend(ppExtensions, &pPortabilityExtensionName);

  info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  #endif

  #ifdef OGE_DEBUG
  const char *pValidationLayerName = "VK_LAYER_KHRONOS_validation";

  if (isValidationLayerSupported(pValidationLayerName)) {
    info.enabledLayerCount   = 1;
    info.ppEnabledLayerNames = &pValidationLayerName;

    const char *pDebugExtensionName = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    ppExtensions = ogeDArrayAppend(ppExtensions, &pDebugExtensionName);
  }
  else {
    info.enabledLayerCount   = 0;
    info.ppEnabledLayerNames = 0;
  }
  #endif

  info.enabledExtensionCount   = ogeDArrayLength(ppExtensions);
  info.ppEnabledExtensionNames = ppExtensions;

  const VkResult result = vkCreateInstance(
    &info, s_rendererState.pAllocator, &s_rendererState.instance);

  ogeDArrayDeallocate(ppExtensions);

  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan instance: %d.", result);
    return OGE_FALSE;
  }

  OGE_TRACE("Vulkan instance created.");
  return OGE_TRUE;
}

b8 createSurface() {
  OGE_TRACE("Creating Vulkan surface.");

  const VkResult result =
    ogePlatformCreateVkSurface(s_rendererState.instance,
      s_rendererState.pAllocator, &s_rendererState.surface);

  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan instance: %d.", result);
    return OGE_FALSE;
  }

  OGE_TRACE("Vulkan surface created.");
  return OGE_TRUE;
}

b8 isPhysicalDeviceSuitable(VkPhysicalDevice device) {
  #ifdef OGE_DEBUG
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  #endif

  OGE_TRACE("Checking GPU %s for suitability.",
            deviceProperties.deviceName);

  // Queue family indicies
  OgeQueueFamilyIndicies queueFamilyIndicies;
  ogeQuerryQueueFamilyIndicies(
    device, s_rendererState.surface, &queueFamilyIndicies);

  if (queueFamilyIndicies.graphics == -1 ||
      queueFamilyIndicies.transfer == -1 ||
      queueFamilyIndicies.compute  == -1 ||
      queueFamilyIndicies.present  == -1) {
    OGE_TRACE("GPU %s failed on queue family indicies check.",
              deviceProperties.deviceName);
    return OGE_FALSE;
  }

  // Swapchain support
  OgeSwapchainSupport swapchainSupport = {
    .pFormats = 0,
    .pPresentModes = 0,
  };
  ogeQuerrySwapchainSupport(
    device, s_rendererState.surface, &swapchainSupport);

  if (swapchainSupport.formatCount == 0 ||
      swapchainSupport.presentModeCount == 0) {
    OGE_TRACE("GPU %s failed on swapchain support check.");

    ogeDeallocate(swapchainSupport.pFormats);
    ogeDeallocate(swapchainSupport.pPresentModes);

    return OGE_FALSE;
  }

  // Device extensions
  u32 availableExtensionCount;
  vkEnumerateDeviceExtensionProperties(
    device, 0, &availableExtensionCount, 0);

  if (availableExtensionCount == 0) {
    OGE_TRACE("GPU %s failed on device extensions check.");
    return OGE_FALSE;
  }

  ogeDeallocate(swapchainSupport.pFormats);
  ogeDeallocate(swapchainSupport.pPresentModes);

  OGE_TRACE("GPU %s is suitable.",
            deviceProperties.deviceName);
  return OGE_TRUE;
}

b8 selectPhysicalDevice() {
  OGE_TRACE("Selecting GPU.");

  u32 deviceCount = 0;
  vkEnumeratePhysicalDevices(s_rendererState.instance, &deviceCount, 0);

  if (deviceCount == 0) {
    OGE_ERROR("Failed to find GPUs with Vulkan support.");
    return OGE_FALSE;
  }

  VkPhysicalDevice pDevices[deviceCount];
  vkEnumeratePhysicalDevices(s_rendererState.instance, &deviceCount, pDevices);

  for (uint32_t i = 0; i < deviceCount; ++i) {
    if (!isPhysicalDeviceSuitable(pDevices[i])) { continue; }

    s_rendererState.physicalDevice = pDevices[i];

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(pDevices[i], &deviceProperties);

    OGE_INFO("Selected GPU: %s.", deviceProperties.deviceName);
    return OGE_TRUE;
  }

  OGE_ERROR("Failed to find a suitable GPU.");
  return OGE_FALSE;
}

b8 ogeRendererInit(OgeRendererInitInfo *pInitInfo) {
  if (s_rendererState.initialized) {
    OGE_WARN("Trying to initialize renderer while it's already initialized.");
    return OGE_TRUE;
  }

  OGE_TRACE("Initializing renderer.");

  if (!createInstance(pInitInfo)) { return OGE_FALSE; }

  #ifdef OGE_DEBUG
  createDebugMessenger();
  #endif

  if (!createSurface()) { return OGE_FALSE; }
  if (!selectPhysicalDevice()) { return OGE_FALSE; }

  s_rendererState.initialized = OGE_TRUE;
  OGE_TRACE("Renderer initialized.");
  return OGE_TRUE;
}

void ogeRendererTerminate() {
  if (!s_rendererState.initialized) {
    OGE_WARN("Trying to terminate renderer while it's already terminated.");
    return;
  }

  OGE_TRACE("Terminating Vulkan renderer.");

  vkDestroySurfaceKHR(s_rendererState.instance,
                      s_rendererState.surface,
                      s_rendererState.pAllocator);
  OGE_TRACE("Vulkan surface destroyed.");

  #ifdef OGE_DEBUG
  vkDestroyDebugUtilsMessengerEXT(
    s_rendererState.instance, s_rendererState.debugMessenger,
    s_rendererState.pAllocator);
  OGE_TRACE("Vulkan debug messender destroyed.");
  #endif

  vkDestroyInstance(s_rendererState.instance, 0);
  OGE_TRACE("Vulkan instance destroyed.");

  s_rendererState.initialized = OGE_FALSE;
  OGE_INFO("Renderer terminated.");
}
