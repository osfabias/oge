#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "oge/defines.h"

typedef struct queueFamilyIndicies {
  u32 graphics;
  u32 transfer;
  u32 compute;
  u32 present;
} queueFamilyIndicies;

typedef struct swapchainSupport {
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  u32                      formatCount;
  VkSurfaceFormatKHR       pFormats[128];
  u32                      presentModeCount;
  VkPresentModeKHR         pPresentModes[16];
} swapchainSupport;
