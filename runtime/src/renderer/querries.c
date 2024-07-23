#include <vulkan/vulkan.h>

#include "oge/defines.h"
#include "oge/core/memory.h"
#include "oge/core/assertion.h"
#include "oge/renderer/querries.h"
#include "oge/renderer/renderer-types.h"

// TODO: improve querry script 
void ogeQuerryQueueFamilyIndicies(
  VkPhysicalDevice device,
  VkSurfaceKHR surface,
  OgeQueueFamilyIndicies *pQueueFamilyIndicies) {

  OGE_ASSERT(pQueueFamilyIndicies,
             "querryQueueFamilyIndicies(): pQueueFamilyIndicies must be a non NULL value.");

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


void ogeQuerrySwapchainSupport(
  VkPhysicalDevice device,
  VkSurfaceKHR surface,
  OgeSwapchainSupport *pSwapchainSupport) {

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

