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


