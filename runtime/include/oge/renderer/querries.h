#pragma once

#include <vulkan/vulkan.h>

#include "oge/defines.h"
#include "oge/renderer/renderer-types.h"

/**
 * @brief Querries queue family indicies.
 */
void ogeQuerryQueueFamilyIndicies(
  VkPhysicalDevice device,
  VkSurfaceKHR surface,
  OgeQueueFamilyIndicies *pQueueFamilyIndicies);

/**
 * @brief Querries swapchain support
 *
 * User should deallocate an output of the function.
 */
void ogeQuerrySwapchainSupport(
  VkPhysicalDevice device,
  VkSurfaceKHR surface,
  OgeSwapchainSupport *pSwapchainSupport);
