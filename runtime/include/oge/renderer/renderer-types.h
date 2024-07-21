#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "oge/defines.h"

typedef struct OgeQueueFamilyIndicies {
  u32 graphics;
  u32 transfer;
  u32 compute;
  u32 present;
} OgeQueueFamilyIndicies;

typedef struct OgeSwapchainSupport {
  VkSurfaceCapabilitiesKHR  surfaceCapabilities;
  u32                       formatCount;
  VkSurfaceFormatKHR       *pFormats;
  u32                       presentModeCount;
  VkPresentModeKHR         *pPresentModes;
} OgeSwapchainSupport;
