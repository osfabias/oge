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

