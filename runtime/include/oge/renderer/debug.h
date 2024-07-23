#pragma once

#include <vulkan/vulkan.h>

#include "oge/core/logging.h"

VkResult vkCreateDebugUtilsMessengerEXT(
  VkInstance instance,
  const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
  const VkAllocationCallbacks *pAllocator,
  VkDebugUtilsMessengerEXT *pDebugMessenger);

void vkDestroyDebugUtilsMessengerEXT(
  VkInstance instance,
  VkDebugUtilsMessengerEXT debugMessenger,
  const VkAllocationCallbacks *pAllocator);

b8 ogeIsValidationLayerSupported(const char *pLayerName);

// NOTE: This function should be located at the header file,
// because in another case, when filling the structure
// VkDebugUtilsMessengerCreateInfoEX the compiler throws an
// "undefined symbol _ogeVulkanDebugCallback" error
static VKAPI_ATTR VKAPI_CALL VkBool32 ogeVulkanDebugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData) {

  switch(messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      OGE_WARN("Vulkan: %s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      OGE_ERROR("Vulkan: %s", pCallbackData->pMessage);
      break;
    default:
      ;
  }

  return VK_FALSE;

}


