#include <vulkan/vulkan.h>

#include "oge/defines.h"
#include "oge/core/memory.h"
#include "oge/core/assertion.h"
#include "oge/renderer/querries.h"
#include "oge/renderer/renderer-types.h"

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

  u8 minScore = 255, score;
  for (u32 i = 0; i < queueFamilyCount; ++i) {
    score = 0;

    // Graphics queue
    if (familyProperties[i].queueFlags &
        VK_QUEUE_GRAPHICS_BIT) {
      pQueueFamilyIndicies->graphics = i;
      ++score;
    }

    // Compute queue
    if (familyProperties[i].queueFlags &
        VK_QUEUE_COMPUTE_BIT) {
      pQueueFamilyIndicies->compute = i;
      ++score;
    }

    // Transfer queue
    // NOTE: We want the transfer queue to be separated from
    // the other queues, so a family with the least score is
    // our choise
    if (familyProperties[i].queueFlags &
        VK_QUEUE_TRANSFER_BIT) {
      if (score <= minScore) {
        pQueueFamilyIndicies->transfer = i;
        score = minScore;
      }
    }

    // Present queue
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

  OGE_ASSERT(!pSwapchainSupport->pFormats,
             "querrySwapchainSupport(): pSwapchainSupport->pFormats must be a NULL value.");

  OGE_ASSERT(!pSwapchainSupport->pPresentModes,
             "querrySwapchainSupport(): pSwapchainSupport->pPresentModes must be a NULL value.");


  // Surface capabilities
  VkSurfaceCapabilitiesKHR surfaceCapabilities; vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    device, surface, &pSwapchainSupport->surfaceCapabilities);

  // Surface formats
  vkGetPhysicalDeviceSurfaceFormatsKHR(
    device, surface, &pSwapchainSupport->formatCount, 0);

  if (pSwapchainSupport->formatCount != 0) {
    pSwapchainSupport->pFormats =
      ogeAllocate(pSwapchainSupport->formatCount * sizeof(VkSurfaceFormatKHR), 
                  OGE_MEMORY_TAG_ARRAY);
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

    pSwapchainSupport->pPresentModes =
      ogeAllocate(
        pSwapchainSupport->presentModeCount * sizeof(VkPresentModeKHR),
        OGE_MEMORY_TAG_ARRAY);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
      device, surface, &pSwapchainSupport->presentModeCount,
      pSwapchainSupport->pPresentModes);
  }
}

