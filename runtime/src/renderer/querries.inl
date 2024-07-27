#pragma once

#include <vulkan/vulkan.h>

#include "oge/defines.h"

#include "types.inl"

// btw don't sure, that we can call this function a querry typa func, so...
// think about it later
OGE_INLINE void querryQueueFamilyIndicies(
  VkPhysicalDevice device,
  VkSurfaceKHR surface,
  queueFamilyIndicies *pQueueFamilyIndicies) {

  pQueueFamilyIndicies->graphics = -1;
  pQueueFamilyIndicies->transfer = -1;
  pQueueFamilyIndicies->compute  = -1;
  pQueueFamilyIndicies->present  = -1;

  u32 queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(
    device, &queueFamilyCount, 0);
  VkQueueFamilyProperties familyProperties[queueFamilyCount];
  vkGetPhysicalDeviceQueueFamilyProperties(
    device, &queueFamilyCount, familyProperties);

  u8 minScore = 255;
  for (u32 i = 0; i < queueFamilyCount; ++i) {
    // Graphics queue
    if (pQueueFamilyIndicies->graphics == -1 &&
       (familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
      pQueueFamilyIndicies->graphics = i;
      continue;
    }

    // Transfer queue
    if (pQueueFamilyIndicies->transfer == -1 &&
       (familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)) {
        pQueueFamilyIndicies->transfer = i;
      continue;
    }

    // Compute queue
    if (pQueueFamilyIndicies->compute == -1 &&
       (familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)) {
      pQueueFamilyIndicies->compute = i;
      continue;
    }

    // Present queue
    if (pQueueFamilyIndicies->present != -1) { continue; }

    VkBool32 isPresentSupported = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                         &isPresentSupported);
    if (isPresentSupported) {
      pQueueFamilyIndicies->present = i;
    }
  }
}

void querrySwapchainSupport(
  VkPhysicalDevice device,
  VkSurfaceKHR surface,
  swapchainSupport *pSwapchainSupport) {

  // Surface capabilities
  VkSurfaceCapabilitiesKHR surfaceCapabilities; vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    device, surface, &pSwapchainSupport->surfaceCapabilities);

  // Surface formats
  vkGetPhysicalDeviceSurfaceFormatsKHR(
    device, surface, &pSwapchainSupport->formatCount, 0);

  if (pSwapchainSupport->formatCount != 0) {
    vkGetPhysicalDeviceSurfaceFormatsKHR(
      device, surface, &pSwapchainSupport->formatCount, 
      pSwapchainSupport->pFormats);
  }

  // Present modes
  vkGetPhysicalDeviceSurfacePresentModesKHR(
    device, surface, &pSwapchainSupport->presentModeCount, 0);

  if (pSwapchainSupport->presentModeCount != 0) {
    vkGetPhysicalDeviceSurfacePresentModesKHR(
      device, surface, &pSwapchainSupport->presentModeCount, 0);

    vkGetPhysicalDeviceSurfacePresentModesKHR(
      device, surface, &pSwapchainSupport->presentModeCount,
      pSwapchainSupport->pPresentModes);
  }
}

