#include <string.h>

#include <vulkan/vulkan.h>

#include "oge/renderer/debug.h"

VkResult vkCreateDebugUtilsMessengerEXT(
  VkInstance instance,
  const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
  const VkAllocationCallbacks *pAllocator,
  VkDebugUtilsMessengerEXT *pDebugMessenger) {

  PFN_vkCreateDebugUtilsMessengerEXT func =
    (PFN_vkCreateDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

  if (func) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }

  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void vkDestroyDebugUtilsMessengerEXT(
  VkInstance instance,
  VkDebugUtilsMessengerEXT debugMessenger,
  const VkAllocationCallbacks *pAllocator) {

   PFN_vkDestroyDebugUtilsMessengerEXT func =
    (PFN_vkDestroyDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

  if (func) {
    func(instance, debugMessenger, pAllocator);
  }
}

b8 ogeIsValidationLayerSupported(const char *pLayerName) {
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

