#include <stdio.h>
#include <string.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "oge/defines.h"
#include "oge/core/memory.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/assertion.h"
#include "oge/containers/darray.h"
#include "oge/renderer/renderer.h"

#include "types.h"
#include "querries.h"

#ifdef OGE_DEBUG
#include "debug.h"
#endif

#define MAX_FRAMES_IN_FLIGHT 2

// TODO: write custom allocators for vulkan
static struct {
  b8 initialized;

  VkInstance instance;

  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  swapchainSupport swapchainSupport;
  VkFormat swapchainFormat;
  VkExtent2D swapchainExtent;
  u32 swapchainImageCount;
  VkImage *swapchainImages;
  VkImageView *swapchainImageViews;
  VkFramebuffer *framebuffers;

  VkPhysicalDevice physicalDevice;
  VkPhysicalDeviceFeatures physicalDeviceFeatures;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
  queueFamilyIndicies queueFamilyIndicies;

  VkDevice logicalDevice;
  struct {
    VkQueue graphics;
    VkQueue transfer;
    VkQueue compute;
    VkQueue present;
  } queues;

  VkRenderPass renderPass;

  VkPipeline graphicsPipeline;
  VkPipelineLayout graphicsPipelineLayout;

  struct {
    VkCommandPool graphics;
    VkCommandPool transfer;
    VkCommandPool compute;
    VkCommandPool present;
  } commandPools;

  struct {
    VkCommandBuffer *graphics;
    VkCommandBuffer *transfer;
    VkCommandBuffer *compute;
    VkCommandBuffer *present;
  } commandBuffers;

  VkSemaphore *imageAvailableSemaphores;
  VkSemaphore *renderFinishedSemaphores;
  VkFence *inFlightFences;

  u32 currentFrameIndex;
  u32 currentImageIndex;
  VkClearValue frameClearColor;

  VkAllocationCallbacks *pAllocator;
  #ifdef OGE_DEBUG
  VkDebugUtilsMessengerEXT debugMessenger;
  #endif
} s_rendererState = {
  .initialized       = OGE_FALSE,
  .currentFrameIndex = 0,

  .pAllocator = 0, // temporary, while custom allocator isn't written
};

const char *s_ppRequiredDeviceExtensions[] = {
#if defined(OGE_PLATFORM_APPLE)
  #define REQUIRED_DEVICE_EXTENSIONS_COUNT 2

  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  "VK_KHR_portability_subset",
#elif defined(OGE_PLATFORM_LINUX)
  #define REQUIRED_DEVICE_EXTENSIONS_COUNT 1

  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#else
  #error "Device extensions for this platform aren't defined."
#endif
};

#ifdef OGE_DEBUG
void createDebugMessenger() {
  OGE_TRACE("Creating Vulkan debug messenger.");

  VkDebugUtilsMessengerCreateInfoEXT createInfo = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    .messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = ogeVulkanDebugCallback,
    .pUserData = 0,
  };

  const VkResult result = vkCreateDebugUtilsMessengerEXT(
    s_rendererState.instance, &createInfo,
    s_rendererState.pAllocator, &s_rendererState.debugMessenger);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan debug messenger.");
    return;
  }

  OGE_TRACE("Vulkan debug messenger created.");
}
#endif

OGE_INLINE b8 isValidationLayerSupported(const char *pLayerName) {
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

/************************************************
 *              creation functions              *
 ************************************************/
OGE_INLINE b8 createInstance(const OgeRendererInitInfo *pInitInfo) {
  VkApplicationInfo applicationInfo = {
    .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .apiVersion         = VK_API_VERSION_1_3,
    .pApplicationName   = pInitInfo->applicationName,
    .applicationVersion = pInitInfo->applicationVersion,
    .pEngineName        = "Osfabias Game Engine",
    .engineVersion      = VK_MAKE_VERSION(OGE_VERSION_MAJOR,
                                          OGE_VERSION_MINOR, 
                                          OGE_VERSION_PATCH),
    .pNext              = 0,
  };

  VkInstanceCreateInfo info = {
    .sType               = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext               = 0,
    .flags               = 0,
    .pApplicationInfo    = &applicationInfo,
    .enabledLayerCount   = 0,
    .ppEnabledLayerNames = 0,
  };

  u32 extensionsCount;
  ogePlatformGetDeviceExtensions(&extensionsCount, 0);
  const char* extensionNames[extensionsCount];
  ogePlatformGetDeviceExtensions(&extensionsCount, extensionNames);

  const char* *extensions =
    ogeDArrayAlloc(extensionsCount, sizeof(char*));
  extensions =
    ogeDArrayExtend(extensions, extensionNames, extensionsCount);

  #ifdef OGE_PLATFORM_APPLE
  const char *pPortabilityExtensionName = 
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
  extensions =
    ogeDArrayAppend(extensions, &pPortabilityExtensionName);

  info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  #endif

  #ifdef OGE_DEBUG
  const char *pValidationLayerName = "VK_LAYER_KHRONOS_validation";

  if (isValidationLayerSupported(pValidationLayerName)) {
    info.enabledLayerCount   = 1;
    info.ppEnabledLayerNames = &pValidationLayerName;

    const char *pDebugExtensionName = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    extensions = ogeDArrayAppend(extensions, &pDebugExtensionName);
  }
  #endif

  info.enabledExtensionCount   = ogeDArrayLength(extensions);
  info.ppEnabledExtensionNames = extensions;

  const VkResult result = 
    vkCreateInstance(&info, s_rendererState.pAllocator,
                     &s_rendererState.instance);

  ogeDArrayFree(extensions);

  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan instance: %d.", result);
    return OGE_FALSE;
  }

  OGE_TRACE("Vulkan instance created.");
  return OGE_TRUE;
}

OGE_INLINE b8 createSurface() {
  OGE_TRACE("Creating Vulkan surface.");

  const VkResult result =
    ogePlatformCreateSurface(s_rendererState.instance,
      s_rendererState.pAllocator, &s_rendererState.surface);

  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan instance: %d.", result);
    return OGE_FALSE;
  }

  OGE_TRACE("Vulkan surface created.");
  return OGE_TRUE;
}

OGE_INLINE b8 isGPUSuitable(VkPhysicalDevice device) {
  // Queue family indicies
  queueFamilyIndicies queueFamilyIndicies;
  querryQueueFamilyIndicies(
    device, s_rendererState.surface, &queueFamilyIndicies);

  if (queueFamilyIndicies.graphics == -1 ||
      queueFamilyIndicies.transfer == -1 ||
      queueFamilyIndicies.compute  == -1 ||
      queueFamilyIndicies.present  == -1) {
    return OGE_FALSE;
  }

  // Swapchain support
  swapchainSupport swapchainSupport;
  querrySwapchainSupport(
    device, s_rendererState.surface, &swapchainSupport);

  if (swapchainSupport.formatCount      == 0 ||
      swapchainSupport.presentModeCount == 0) {
    return OGE_FALSE;
  }

  // Device extensions
  u32 availableExtensionCount;
  vkEnumerateDeviceExtensionProperties(
    device, 0, &availableExtensionCount, 0);

  if (availableExtensionCount == 0) { return OGE_FALSE; }

  VkExtensionProperties pDeviceExtensions[availableExtensionCount]; 
  vkEnumerateDeviceExtensionProperties(
    device, 0, &availableExtensionCount, pDeviceExtensions);

  b8 extensionFound;
  for (u32 i = 0; i < REQUIRED_DEVICE_EXTENSIONS_COUNT; ++i) {
    extensionFound = OGE_FALSE;

    for (u32 j = 0; j < availableExtensionCount; ++j) {
      if (strcmp(s_ppRequiredDeviceExtensions[i],
                 pDeviceExtensions[j].extensionName) == 0) {
        extensionFound = OGE_TRUE;
        break;
      }
    }

    if (!extensionFound) { return OGE_FALSE; }
  }

  // Sampler anisotropy
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  if (!deviceFeatures.samplerAnisotropy) { return OGE_FALSE; }

  return OGE_TRUE;
}

// yo! bro, I don't think that requirements is the our way of things
//
// we need to get every feature we can and than use ALL OF EM!
// if we were unable to find something - we just wouldn't use that
//
// but do wee really need these requirements if we are already
// know which things are really necessery for engine to fire up?
OGE_INLINE b8 selectGPU(/* requirements ? */) {
  OGE_TRACE("Selecting GPU.");

  u32 deviceCount = 0;
  vkEnumeratePhysicalDevices(s_rendererState.instance, &deviceCount, 0);

  if (deviceCount == 0) {
    OGE_ERROR("Failed to find GPUs with Vulkan support.");
    return OGE_FALSE;
  }

  VkPhysicalDevice devices[deviceCount];
  vkEnumeratePhysicalDevices(s_rendererState.instance, &deviceCount, devices);

  for (u32 i = 0; i < deviceCount; ++i) {
    if (!isGPUSuitable(devices[i])) { continue; }

    s_rendererState.physicalDevice = devices[i];

    VkPhysicalDeviceFeatures deviceFeatures;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

    vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);
    vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
    vkGetPhysicalDeviceMemoryProperties(
      devices[i], &deviceMemoryProperties);

    s_rendererState.physicalDeviceFeatures = deviceFeatures;
    s_rendererState.physicalDeviceProperties = deviceProperties;
    s_rendererState.physicalDeviceMemoryProperties = 
      deviceMemoryProperties;

    querryQueueFamilyIndicies(
      devices[i], s_rendererState.surface,
      &s_rendererState.queueFamilyIndicies);

    querrySwapchainSupport(
      devices[i], s_rendererState.surface,
      &s_rendererState.swapchainSupport);

    OGE_INFO(
      "Selected GPU:\n\n\t➜ Name:               %s\n\t➜ Driver version:     %d.%d.%d\n\t➜ Vulkan API version: %d.%d.%d\n", 
      deviceProperties.deviceName,

      VK_VERSION_MAJOR(deviceProperties.driverVersion),
      VK_VERSION_MINOR(deviceProperties.driverVersion),
      VK_VERSION_PATCH(deviceProperties.driverVersion),

      VK_VERSION_MAJOR(deviceProperties.apiVersion),
      VK_VERSION_MINOR(deviceProperties.apiVersion),
      VK_VERSION_PATCH(deviceProperties.apiVersion)
    );
    return OGE_TRUE;
  }

  OGE_ERROR("Failed to find a suitable GPU.");
  return OGE_FALSE;
}

OGE_INLINE b8 createLogicalDevice() {
  // Queues
  const u32 queueFamilyIndicies[] = {
    s_rendererState.queueFamilyIndicies.graphics,
    s_rendererState.queueFamilyIndicies.transfer,
    s_rendererState.queueFamilyIndicies.compute,
    s_rendererState.queueFamilyIndicies.present,
  };

  VkDeviceQueueCreateInfo queueCreateInfos[4];

  const f32 queuePriopity = 1.0f;
  for (u32 i = 0; i < 4; ++i) {
    VkDeviceQueueCreateInfo *ptr  = queueCreateInfos + i;

    ptr->sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    ptr->flags            = 0;
    ptr->queueFamilyIndex = queueFamilyIndicies[i];
    ptr->queueCount       = 1;
    ptr->pQueuePriorities = &queuePriopity;
    ptr->pNext            = 0;
  }

  // Device features
  VkPhysicalDeviceFeatures deviceFeatures;
  ogeMemSet(&deviceFeatures, VK_FALSE, sizeof(deviceFeatures));
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  // Creation
  const VkDeviceCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .flags = 0,

    // Deprecated
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = 0,

    .enabledExtensionCount = REQUIRED_DEVICE_EXTENSIONS_COUNT,
    .ppEnabledExtensionNames = s_ppRequiredDeviceExtensions,

    .pEnabledFeatures = &deviceFeatures,

    .queueCreateInfoCount = 4,
    .pQueueCreateInfos = queueCreateInfos,

    .pNext = 0,
  };

  const VkResult result =
    vkCreateDevice(s_rendererState.physicalDevice, &info,
                   s_rendererState.pAllocator,
                   &s_rendererState.logicalDevice);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan logical device: %d.", result);
    return OGE_FALSE;
  }
 
  return OGE_TRUE;
}

void getQueues() {
  vkGetDeviceQueue(
    s_rendererState.logicalDevice,
    s_rendererState.queueFamilyIndicies.graphics,
    0, &s_rendererState.queues.graphics);

  vkGetDeviceQueue(
    s_rendererState.logicalDevice,
    s_rendererState.queueFamilyIndicies.transfer,
    0, &s_rendererState.queues.transfer);

  vkGetDeviceQueue(
    s_rendererState.logicalDevice,
    s_rendererState.queueFamilyIndicies.compute,
    0, &s_rendererState.queues.compute);

  vkGetDeviceQueue(
    s_rendererState.logicalDevice,
    s_rendererState.queueFamilyIndicies.present,
    0, &s_rendererState.queues.present);
  OGE_TRACE("Vulkan queues obtained.");
}

OGE_INLINE VkSurfaceFormatKHR chooseSurfaceFormat(
  VkSurfaceFormatKHR *formats, u32 formatsCount) {

  for (u32 i = 0; i < formatsCount; ++i) {
    if (formats[i].format != VK_FORMAT_R8G8B8_SRGB) {
      continue;
    }

    if (formats[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      continue;
    }

    return formats[i];
  }

  return formats[0];
}

OGE_INLINE VkPresentModeKHR choosePresentMode(
  VkPresentModeKHR *presentModes, u32 presentModesCount) {

  for (u32 i = 0; i < presentModesCount; ++i) {
    if (presentModes[i] != VK_PRESENT_MODE_MAILBOX_KHR) { continue; }
    return presentModes[i];
  }

  return presentModes[0];
}

OGE_INLINE VkExtent2D chooseExtent(VkSurfaceCapabilitiesKHR surfaceCapabilities) {
  return surfaceCapabilities.currentExtent;
}

OGE_INLINE b8 createSwapchain() {
  swapchainSupport swapchainSupport;
  querrySwapchainSupport(s_rendererState.physicalDevice,
                         s_rendererState.surface, &swapchainSupport);

  const VkSurfaceFormatKHR surfaceFormat =
    chooseSurfaceFormat(swapchainSupport.pFormats,
                        swapchainSupport.formatCount);

  const VkPresentModeKHR presentMode =
    choosePresentMode(swapchainSupport.pPresentModes,
                      swapchainSupport.presentModeCount);

  s_rendererState.swapchainExtent
    = chooseExtent(swapchainSupport.surfaceCapabilities);

  s_rendererState.swapchainFormat = surfaceFormat.format;

  // Creation
  const u32 queueFamilyIndicies[] = {
    s_rendererState.queueFamilyIndicies.graphics,
    s_rendererState.queueFamilyIndicies.transfer,
    s_rendererState.queueFamilyIndicies.compute,
    s_rendererState.queueFamilyIndicies.present
  };

  const VkSwapchainCreateInfoKHR info = {
    .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .flags                 = 0,
    .surface               = s_rendererState.surface,
    .minImageCount         = swapchainSupport.surfaceCapabilities.minImageCount,
    .imageFormat           = s_rendererState.swapchainFormat,
    .imageColorSpace       = surfaceFormat.colorSpace,
    .imageExtent           = s_rendererState.swapchainExtent,
    .imageArrayLayers      = 1,
    .imageSharingMode      = VK_SHARING_MODE_CONCURRENT,
    .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .preTransform          =
      swapchainSupport.surfaceCapabilities.currentTransform,
    .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .queueFamilyIndexCount = 4,
    .pQueueFamilyIndices   = queueFamilyIndicies,
    .presentMode           = presentMode,
    .clipped               = VK_TRUE,
    .oldSwapchain          = VK_NULL_HANDLE,
    .pNext                 = 0,
  };

  const VkResult result = vkCreateSwapchainKHR(
    s_rendererState.logicalDevice, &info, s_rendererState.pAllocator,
    &s_rendererState.swapchain);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan swapchain.");
    return OGE_FALSE;
  }
  OGE_TRACE("Vulkan swapchain created.");
  return OGE_TRUE;
}

void getSwapchainImages() {
  vkGetSwapchainImagesKHR(
    s_rendererState.logicalDevice, s_rendererState.swapchain,
    &s_rendererState.swapchainImageCount, 0);

  s_rendererState.swapchainImages =
    ogeAlloc(sizeof(VkImage) * s_rendererState.swapchainImageCount,
             OGE_MEMORY_TAG_ARRAY);

  vkGetSwapchainImagesKHR(
    s_rendererState.logicalDevice, s_rendererState.swapchain,
    &s_rendererState.swapchainImageCount, s_rendererState.swapchainImages);

  OGE_TRACE("Vulkan swapchain images obtained.");
}

b8 createSwapchainImageViews() {
  s_rendererState.swapchainImageViews =
    ogeAlloc(sizeof(VkImageView) * s_rendererState.swapchainImageCount,
             OGE_MEMORY_TAG_ARRAY);

  for (u32 i = 0; i < s_rendererState.swapchainImageCount; i++) {
    VkImageViewCreateInfo info = {
      .sType                           =
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image                           = s_rendererState.swapchainImages[i],
      .viewType                        = VK_IMAGE_VIEW_TYPE_2D,
      .format                          = s_rendererState.swapchainFormat,
      .components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY,
      .components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY,
      .components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY,
      .components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY,
      .subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
      .subresourceRange.baseMipLevel   = 0,
      .subresourceRange.levelCount     = 1,
      .subresourceRange.baseArrayLayer = 0,
      .subresourceRange.layerCount     = 1,
    };
    VkResult result =
      vkCreateImageView(s_rendererState.logicalDevice, &info,
                        s_rendererState.pAllocator,
                        &s_rendererState.swapchainImageViews[i]);
    if (result != VK_SUCCESS) {
      OGE_TRACE("Failed to create image views.");
      return OGE_FALSE;
    }
  }

  OGE_TRACE("Vulkan swapchain image views created.");
  return OGE_TRUE;
}

b8 createRenderPass() {
  VkAttachmentDescription colorAttachment = {
    .flags          = 0,
    .format         = s_rendererState.swapchainFormat,
    .samples        = VK_SAMPLE_COUNT_1_BIT,
    .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
    .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
    .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  VkAttachmentReference colorAttachmentRef = {
    .attachment = 0,
    .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  };

  VkSubpassDescription subpass = {
    .flags                   = 0,
    .inputAttachmentCount    = 0,
    .pInputAttachments       = 0,
    .pPreserveAttachments    = 0,
    .pDepthStencilAttachment = 0,
    .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
    .colorAttachmentCount    = 1,
    .pColorAttachments       = &colorAttachmentRef,
  };

  VkSubpassDependency dependency = {
    .srcSubpass      = VK_SUBPASS_EXTERNAL,
    .dstSubpass      = 0,
    .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    .srcAccessMask   = 0,
    .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    .dependencyFlags = 0,
  };

  VkRenderPassCreateInfo info = {
    .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    .flags           = 0,
    .attachmentCount = 1,
    .pAttachments    = &colorAttachment,
    .subpassCount    = 1,
    .pSubpasses      = &subpass,
    .dependencyCount = 1,
    .pDependencies   = &dependency,
    .pNext           = 0,
  };

  VkResult result =
    vkCreateRenderPass(s_rendererState.logicalDevice, &info,
                       0, &s_rendererState.renderPass);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan render pass.");
    return OGE_FALSE;
  }
  OGE_TRACE("Vulkan render pass created.");
  return OGE_TRUE;
}

b8 createGraphicsPipelineLayout() {
  const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
    .sType                  =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext                  = 0,
    .flags                  = 0,
    .pushConstantRangeCount = 0,
    .pPushConstantRanges    = 0,
    .setLayoutCount         = 0,
    .pSetLayouts            = 0,
  };

  VkResult result =
    vkCreatePipelineLayout(s_rendererState.logicalDevice,
                           &pipelineLayoutInfo,
                           s_rendererState.pAllocator,
                           &s_rendererState.graphicsPipelineLayout);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan graphics pipeline layout.");
    return OGE_FALSE;
  }
  OGE_TRACE("Vulkan graphics pipeline layout created.");
  return OGE_TRUE;
}

b8 createShaderModule(const char *fileName, VkShaderModule *module) {
  OGE_WARN("createShaderModule() uses standart C file I/O functions, replace them with OGE file I/O system calls.");

  FILE *shaderCodeFile = fopen(fileName, "rb");

  if (!shaderCodeFile) {
    OGE_ERROR("createShaderModule(): failed to open \"%s\" file.",
              fileName);
    return OGE_FALSE;
  }

  fseek(shaderCodeFile, 0, SEEK_END);
  const u64 shaderCodeSize = ftell(shaderCodeFile);
  rewind(shaderCodeFile);

  const char *shaderCode[shaderCodeSize];
  fread(shaderCode, sizeof(shaderCode), 1, shaderCodeFile);

  fclose(shaderCodeFile);

  const VkShaderModuleCreateInfo info = {
    .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext    = 0,
    .flags    = 0,
    .codeSize = shaderCodeSize,
    .pCode    = (u32*)shaderCode,
  };

  const VkResult result =
    vkCreateShaderModule(s_rendererState.logicalDevice, &info,
                         s_rendererState.pAllocator, module);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create shader module: \"%s\".", fileName);
    return OGE_FALSE;
  }
  OGE_TRACE("Shader module created: %s.", fileName);
  return OGE_TRUE;
}

b8 createGraphicsPipeline(const OgeRendererInitInfo *initInfo) {
  VkShaderModule vertShaderModule, fragShaderModule;

  if (!createShaderModule(initInfo->vertexShaderFileName,
                          &vertShaderModule)) {
    return OGE_FALSE;
  }

  if (!createShaderModule(initInfo->fragmentShaderFileName,
                          &fragShaderModule)) {
    return OGE_FALSE;
  }

  const VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
    .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .pNext               = 0,
    .flags               = 0,
    .stage               = VK_SHADER_STAGE_VERTEX_BIT,
    .module              = vertShaderModule,
    .pName               = "main",
    .pSpecializationInfo = 0,
  };

  const VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
    .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .pNext               = 0,
    .flags               = 0,
    .stage               = VK_SHADER_STAGE_FRAGMENT_BIT,
    .module              = fragShaderModule,
    .pName               = "main",
    .pSpecializationInfo = 0,
  };

  const VkPipelineShaderStageCreateInfo shaderStages[] = {
    vertShaderStageInfo,
    fragShaderStageInfo,
  };

  // TODO: move configurable
  OGE_WARN("Currently vertex binding and attribute descriptions are hardcoded in createGraphicsPipeline(), make this configurable.");
  const VkVertexInputBindingDescription vertexBindingDescription = {
    .binding   = 0,
    .stride    = sizeof(f32) * 3,
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  };

  const VkVertexInputAttributeDescription vertexAttributeDescription = {
    .location = 0,
    .binding  = 0,
    .format   = VK_FORMAT_R32G32B32_SFLOAT,
    .offset   = 0,
  };

  const VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {
    .sType                           =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pNext                           = 0,
    .flags                           = 0,
    .vertexBindingDescriptionCount   = 1,
    .pVertexBindingDescriptions      = &vertexBindingDescription,
    .vertexAttributeDescriptionCount = 1,
    .pVertexAttributeDescriptions    = &vertexAttributeDescription,
  };

  const VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {
    .sType                  =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .pNext                  = 0,
    .flags                  = 0,
    .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE,
  };

  const VkPipelineViewportStateCreateInfo viewportStateInfo = {
    .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext         = 0,
    .flags         = 0,
    .viewportCount = 1,
    .pViewports    = 0, // the viewport state is dynamic,
                        // so this member ignored
    .scissorCount  = 1,
    .pScissors     = 0, // the scissor state is dynamic,
                        // so this member ignored
  };

  const VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = {
    .sType                   =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .pNext                   = 0,
    .flags                   = 0,
    .depthClampEnable        = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode             = VK_POLYGON_MODE_FILL,
    .lineWidth               = 1.0f,
    .cullMode                = VK_CULL_MODE_BACK_BIT,
    .frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    .depthBiasEnable         = VK_FALSE,
  };

  const VkPipelineMultisampleStateCreateInfo multisampleStateInfo = {
    .sType                 =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .pNext                 = 0,
    .flags                 = 0,
    .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable   = VK_FALSE,
    .minSampleShading      = 1.0f,
    .pSampleMask           = 0,
    .alphaToCoverageEnable = VK_FALSE,
    .alphaToOneEnable      = VK_FALSE,
  };

  const VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
    .blendEnable         = VK_FALSE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE,
    .colorBlendOp        = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
    .alphaBlendOp        = VK_BLEND_OP_ADD,
    .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT |
                           VK_COLOR_COMPONENT_G_BIT |
                           VK_COLOR_COMPONENT_B_BIT |
                           VK_COLOR_COMPONENT_A_BIT,
  };

  const VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {
    .sType             =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .pNext             = 0,
    .flags             = 0,
    .logicOpEnable     = VK_FALSE,
    .logicOp           = 0,
    .attachmentCount   = 1,
    .pAttachments      = &colorBlendAttachmentState,
    .blendConstants[0] = 1.0f,
    .blendConstants[1] = 1.0f,
    .blendConstants[2] = 1.0f,
    .blendConstants[3] = 1.0f,
  };

  const VkDynamicState dynamicStates[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  const VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
    .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pNext             = 0,
    .flags             = 0,
    .dynamicStateCount = 2,
    .pDynamicStates    = dynamicStates,
  };

  VkGraphicsPipelineCreateInfo pipelineInfo = {
    .sType               =
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext               = 0,
    .flags               = 0,
    .stageCount          = 2,
    .pStages             = shaderStages,
    .pVertexInputState   = &vertexInputStateInfo,
    .pInputAssemblyState = &inputAssemblyStateInfo,
    .pViewportState      = &viewportStateInfo,
    .pRasterizationState = &rasterizationStateInfo,
    .pMultisampleState   = &multisampleStateInfo,
    .pColorBlendState    = &colorBlendStateInfo,
    .pDynamicState       = &dynamicStateInfo,
    .layout              = s_rendererState.graphicsPipelineLayout,
    .renderPass          = s_rendererState.renderPass,
    .subpass             = 0,
    .basePipelineHandle  = VK_NULL_HANDLE,
  };

  const VkResult result =
    vkCreateGraphicsPipelines(s_rendererState.logicalDevice,
                              VK_NULL_HANDLE, 1,
                              &pipelineInfo,
                              s_rendererState.pAllocator,
                              &s_rendererState.graphicsPipeline);

  vkDestroyShaderModule(s_rendererState.logicalDevice,
                        fragShaderModule,
                        s_rendererState.pAllocator);

  vkDestroyShaderModule(s_rendererState.logicalDevice,
                        vertShaderModule,
                        s_rendererState.pAllocator);

  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan graphics pipeline.");
    return OGE_FALSE;
  }
  OGE_TRACE("Vulkan graphics pipeline created.");
  return OGE_TRUE;
}

b8 createFramebuffers() {
  s_rendererState.framebuffers =
    ogeAlloc(sizeof(VkFramebuffer) * s_rendererState.swapchainImageCount,
             OGE_MEMORY_TAG_RENDERER);

  for (u32 i = 0; i < s_rendererState.swapchainImageCount; ++i) {
    const VkFramebufferCreateInfo info = {
      .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext           = 0,
      .flags           = 0,
      .attachmentCount = 1,
      .pAttachments    = &s_rendererState.swapchainImageViews[i],
      .width           = s_rendererState.swapchainExtent.width,
      .height          = s_rendererState.swapchainExtent.height,
      .layers          = 1,
      .renderPass      = s_rendererState.renderPass,
    };

    const VkResult result =
      vkCreateFramebuffer(s_rendererState.logicalDevice, &info,
                          s_rendererState.pAllocator,
                          &s_rendererState.framebuffers[i]);
    if (result != VK_SUCCESS) {
      OGE_ERROR("Failed to create framebuffer: %d.", result);
      return OGE_FALSE;
    }
  }
  OGE_TRACE("Vulkan framebuffers created.");
  return OGE_TRUE;
}

b8 createCommandPools() {
  const VkCommandPoolCreateInfo graphicsPoolInfo = {
    .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext            = 0,
    .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = s_rendererState.queueFamilyIndicies.graphics,
  };

  VkResult result = 
    vkCreateCommandPool(s_rendererState.logicalDevice, &graphicsPoolInfo,
                        s_rendererState.pAllocator,
                        &s_rendererState.commandPools.graphics);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create graphics command pool.");
    return OGE_FALSE;
  }

  const VkCommandPoolCreateInfo transferPoolInfo = {
    .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext            = 0,
    .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = s_rendererState.queueFamilyIndicies.transfer,
  };

  result = vkCreateCommandPool(s_rendererState.logicalDevice, &transferPoolInfo,
                               s_rendererState.pAllocator,
                               &s_rendererState.commandPools.transfer);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create transfer command pool.");
    return OGE_FALSE;
  }

  const VkCommandPoolCreateInfo computePoolInfo = {
    .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext            = 0,
    .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = s_rendererState.queueFamilyIndicies.compute,
  };

  result = vkCreateCommandPool(s_rendererState.logicalDevice, &computePoolInfo,
                               s_rendererState.pAllocator,
                               &s_rendererState.commandPools.compute);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create compute command pool.");
    return OGE_FALSE;
  }

  const VkCommandPoolCreateInfo presentPoolInfo = {
    .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext            = 0,
    .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = s_rendererState.queueFamilyIndicies.present,
  };

  result = vkCreateCommandPool(s_rendererState.logicalDevice, &presentPoolInfo,
                               s_rendererState.pAllocator,
                               &s_rendererState.commandPools.present);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create present command pool.");
    return OGE_FALSE;
  }

  OGE_TRACE("Vulkan command pools created.");
  return OGE_TRUE;
}

b8 createCommandBuffers() {
  // Graphics command buffers
  const VkCommandBufferAllocateInfo graphicsBufferInfo = {
    .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext              = 0,
    .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandPool        = s_rendererState.commandPools.graphics,
    .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
  };

  s_rendererState.commandBuffers.graphics =
    ogeAlloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT,
             OGE_MEMORY_TAG_RENDERER);

  VkResult result =
    vkAllocateCommandBuffers(s_rendererState.logicalDevice,
                             &graphicsBufferInfo,
                             s_rendererState.commandBuffers.graphics);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan graphics command buffers.");
    return OGE_FALSE;
  }

  // Transfer command buffers
  const VkCommandBufferAllocateInfo transferBufferInfo = {
    .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext              = 0,
    .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandPool        = s_rendererState.commandPools.transfer,
    .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
  };

  s_rendererState.commandBuffers.transfer =
    ogeAlloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT,
             OGE_MEMORY_TAG_RENDERER);

  result = vkAllocateCommandBuffers(s_rendererState.logicalDevice,
                                    &transferBufferInfo,
                                    s_rendererState.commandBuffers.transfer);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan transfer command buffers.");
    return OGE_FALSE;
  }

  // Compute command buffers
  const VkCommandBufferAllocateInfo computeBufferInfo = {
    .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext              = 0,
    .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandPool        = s_rendererState.commandPools.compute,
    .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
  };

  s_rendererState.commandBuffers.compute =
    ogeAlloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT,
             OGE_MEMORY_TAG_RENDERER);

  result = vkAllocateCommandBuffers(s_rendererState.logicalDevice,
                                    &computeBufferInfo,
                                    s_rendererState.commandBuffers.compute);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan compute command buffers.");
    return OGE_FALSE;
  }

  // Present command buffers
  const VkCommandBufferAllocateInfo presentBufferInfo = {
    .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext              = 0,
    .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandPool        = s_rendererState.commandPools.present,
    .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
  };

  s_rendererState.commandBuffers.present =
    ogeAlloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT,
             OGE_MEMORY_TAG_RENDERER);

  result = vkAllocateCommandBuffers(s_rendererState.logicalDevice,
                                    &presentBufferInfo,
                                    s_rendererState.commandBuffers.present);
  if (result != VK_SUCCESS) {
    OGE_ERROR("Failed to create Vulkan present command buffers.");
    return OGE_FALSE;
  }
  
  OGE_TRACE("Vulkan command buffers created.");
  return OGE_TRUE;
}

b8 createSyncObjects() {
  const VkSemaphoreCreateInfo semaphoreInfo = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = 0,
    .flags = 0,
  };

  const VkFenceCreateInfo fenceInfo = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = 0,
    .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };

  s_rendererState.imageAvailableSemaphores =
    ogeAlloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT,
             OGE_MEMORY_TAG_RENDERER);

  s_rendererState.renderFinishedSemaphores =
    ogeAlloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT,
             OGE_MEMORY_TAG_RENDERER);

  s_rendererState.inFlightFences =
    ogeAlloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT,
             OGE_MEMORY_TAG_RENDERER);

  for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
  {
    VkResult result1 =
      vkCreateSemaphore(s_rendererState.logicalDevice,
                        &semaphoreInfo,
                        s_rendererState.pAllocator,
                        &s_rendererState.imageAvailableSemaphores[i]);

    VkResult result2 =
      vkCreateSemaphore(s_rendererState.logicalDevice,
                        &semaphoreInfo,
                        s_rendererState.pAllocator,
                        &s_rendererState.renderFinishedSemaphores[i]);

    VkResult result3 =
      vkCreateFence(s_rendererState.logicalDevice,
                    &fenceInfo,
                    s_rendererState.pAllocator,
                    &s_rendererState.inFlightFences[i]);

    if (result1 != VK_SUCCESS ||
        result2 != VK_SUCCESS ||
        result3 != VK_SUCCESS) {
      OGE_ERROR("Failed to create sync objects:");
      return OGE_FALSE;
    }
  }

  OGE_TRACE("Vulkan sync objects created.");
  return OGE_TRUE;
}

b8 ogeRendererInit(const OgeRendererInitInfo *initInfo) {
  OGE_ASSERT(
    !s_rendererState.initialized,
    "Trying to initialize renderer while it's already initialized."
  );

  OGE_WARN("Vulkan uses default allocator, write custom one.");

  const VkClearValue clearColor = {
    .color = {
      initInfo->clearColor.r,
      initInfo->clearColor.g,
      initInfo->clearColor.b,
      initInfo->clearColor.a,
    },
  };
  s_rendererState.frameClearColor = clearColor;

  if (!createInstance(initInfo)) { return OGE_FALSE; }

  #ifdef OGE_DEBUG
  createDebugMessenger();
  #endif

  if (!createSurface())                   { return OGE_FALSE; }
  if (!selectGPU())                       { return OGE_FALSE; }
  if (!createLogicalDevice())             { return OGE_FALSE; }

  getQueues();

  if (!createSwapchain())                 { return OGE_FALSE; }

  getSwapchainImages();

  if (!createSwapchainImageViews())      { return OGE_FALSE; }
  if (!createRenderPass())               { return OGE_FALSE; }
  if (!createGraphicsPipelineLayout())   { return OGE_FALSE; }
  if (!createGraphicsPipeline(initInfo)) { return OGE_FALSE; }
  if (!createFramebuffers())             { return OGE_FALSE; }
  if (!createCommandPools())             { return OGE_FALSE; }
  if (!createCommandBuffers())           { return OGE_FALSE; }
  if (!createSyncObjects())              { return OGE_FALSE; }

  s_rendererState.initialized = OGE_TRUE;
  OGE_INFO("Renderer initialized.");
  return OGE_TRUE;
}

void ogeRendererTerminate() {
  OGE_ASSERT(
    s_rendererState.initialized,
    "Trying to terminate renderer while it's already terminated."
  );

  OGE_TRACE("Terminating Vulkan renderer.");
  
  // Sync objects
  for (u32 i = 0; i < s_rendererState.swapchainImageCount; ++i) {
    vkDestroySemaphore(s_rendererState.logicalDevice,
                       s_rendererState.imageAvailableSemaphores[i],
                       s_rendererState.pAllocator);

    vkDestroySemaphore(s_rendererState.logicalDevice,
                       s_rendererState.renderFinishedSemaphores[i],
                       s_rendererState.pAllocator);

    vkDestroyFence(s_rendererState.logicalDevice,
                   s_rendererState.inFlightFences[i],
                   s_rendererState.pAllocator);
  }

  ogeFree(s_rendererState.imageAvailableSemaphores);
  ogeFree(s_rendererState.renderFinishedSemaphores);
  ogeFree(s_rendererState.inFlightFences);

  // Command buffers
  vkFreeCommandBuffers(s_rendererState.logicalDevice,
                       s_rendererState.commandPools.graphics,
                       s_rendererState.swapchainImageCount,
                       s_rendererState.commandBuffers.graphics);

  vkFreeCommandBuffers(s_rendererState.logicalDevice,
                       s_rendererState.commandPools.transfer,
                       s_rendererState.swapchainImageCount,
                       s_rendererState.commandBuffers.transfer);

  vkFreeCommandBuffers(s_rendererState.logicalDevice,
                       s_rendererState.commandPools.compute,
                       s_rendererState.swapchainImageCount,
                       s_rendererState.commandBuffers.compute);

  vkFreeCommandBuffers(s_rendererState.logicalDevice,
                       s_rendererState.commandPools.present,
                       s_rendererState.swapchainImageCount,
                       s_rendererState.commandBuffers.present);

  ogeFree(s_rendererState.commandBuffers.graphics);
  ogeFree(s_rendererState.commandBuffers.transfer);
  ogeFree(s_rendererState.commandBuffers.compute);
  ogeFree(s_rendererState.commandBuffers.present);

  // Commands pools
  vkDestroyCommandPool(s_rendererState.logicalDevice,
                       s_rendererState.commandPools.graphics,
                       s_rendererState.pAllocator);

  vkDestroyCommandPool(s_rendererState.logicalDevice,
                       s_rendererState.commandPools.transfer,
                       s_rendererState.pAllocator);

  vkDestroyCommandPool(s_rendererState.logicalDevice,
                       s_rendererState.commandPools.compute,
                       s_rendererState.pAllocator);

  vkDestroyCommandPool(s_rendererState.logicalDevice,
                       s_rendererState.commandPools.present,
                       s_rendererState.pAllocator);

  // Frame buffers
  for (u32 i = 0; i < s_rendererState.swapchainImageCount; ++i) {
    vkDestroyFramebuffer(s_rendererState.logicalDevice,
                         s_rendererState.framebuffers[i],
                         s_rendererState.pAllocator);
  }

  vkDestroyPipeline(s_rendererState.logicalDevice,
                    s_rendererState.graphicsPipeline,
                    s_rendererState.pAllocator);

  vkDestroyPipelineLayout(s_rendererState.logicalDevice,
                          s_rendererState.graphicsPipelineLayout,
                          s_rendererState.pAllocator);

  vkDestroyRenderPass(s_rendererState.logicalDevice,
                      s_rendererState.renderPass,
                      s_rendererState.pAllocator);

  // Image views
  for (u32 i = 0; i < s_rendererState.swapchainImageCount; ++i) {
    vkDestroyImageView(s_rendererState.logicalDevice,
                       s_rendererState.swapchainImageViews[i], 
                       s_rendererState.pAllocator);
  }
  ogeFree(s_rendererState.swapchainImageViews);
  ogeFree(s_rendererState.swapchainImages);

  vkDestroySwapchainKHR(s_rendererState.logicalDevice,
                        s_rendererState.swapchain,
                        s_rendererState.pAllocator);

  vkDestroyDevice(s_rendererState.logicalDevice, s_rendererState.pAllocator);

  vkDestroySurfaceKHR(s_rendererState.instance,
                      s_rendererState.surface,
                      s_rendererState.pAllocator);
  OGE_TRACE("Vulkan surface destroyed.");

  #ifdef OGE_DEBUG
  vkDestroyDebugUtilsMessengerEXT(
    s_rendererState.instance, s_rendererState.debugMessenger,
    s_rendererState.pAllocator);
  OGE_TRACE("Vulkan debug messender destroyed.");
  #endif

  vkDestroyInstance(s_rendererState.instance, 0);
  OGE_TRACE("Vulkan instance destroyed.");

  s_rendererState.initialized = OGE_FALSE;
  OGE_INFO("Renderer terminated.");
}

void ogeRendererStartScene() {
  vkWaitForFences(
    s_rendererState.logicalDevice, 1,
    &s_rendererState.inFlightFences[s_rendererState.currentFrameIndex],
    VK_TRUE, UINT64_MAX);

  VkResult result = vkAcquireNextImageKHR(
    s_rendererState.logicalDevice,
    s_rendererState.swapchain,
    UINT64_MAX,
    s_rendererState.
      imageAvailableSemaphores[s_rendererState.currentFrameIndex],
    VK_NULL_HANDLE,
    &s_rendererState.currentImageIndex
  );

  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    // recreateSwapchain();
    OGE_WARN("Swapchain recreation reuquired.");
    return;
  }
  OGE_ASSERT(result == VK_SUCCESS ||
             result == VK_SUBOPTIMAL_KHR,
             "Failed to acquire image.");

  const VkCommandBuffer commandBuffer = 
    s_rendererState.commandBuffers.graphics[s_rendererState.currentFrameIndex];

  vkResetFences(
    s_rendererState.logicalDevice,
    1,
    &s_rendererState.inFlightFences[s_rendererState.currentImageIndex]
  );

  vkResetCommandBuffer(commandBuffer, 0);

  // Begin graphics command buffer
  const VkCommandBufferBeginInfo graphicsCommandBufferBeginInfo = {
    .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext            = 0,
    .flags            = 0,
    .pInheritanceInfo = 0,
  };

  result = vkBeginCommandBuffer(commandBuffer,
                                &graphicsCommandBufferBeginInfo);
  OGE_ASSERT(result == VK_SUCCESS, "Failed to begin Vulkan graphics command buffer.");

  // Begin render pass
  const VkRenderPassBeginInfo renderPassBeginInfo = {
    .sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    .pNext             = 0,
    .renderPass        = s_rendererState.renderPass,
    .framebuffer       =
      s_rendererState.framebuffers[s_rendererState.currentImageIndex],
    .renderArea.offset = {0, 0},
    .renderArea.extent = s_rendererState.swapchainExtent,
    .clearValueCount   = 1,
    .pClearValues      = &s_rendererState.frameClearColor,
  };

  vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    s_rendererState.graphicsPipeline);

  const VkViewport viewport = {
    .x = 0.0f,
    .y = 0.0f,
    .width  = s_rendererState.swapchainExtent.width,
    .height = s_rendererState.swapchainExtent.height,
    .minDepth = 0.0f,
    .maxDepth = 1.0f,
  };
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  const VkRect2D scissor = {
    .offset = {0, 0},
    .extent = s_rendererState.swapchainExtent,
  };
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void ogeRendererEndScene() {
  const VkCommandBuffer commandBuffer = 
    s_rendererState.commandBuffers.graphics[s_rendererState.currentFrameIndex];
  vkCmdEndRenderPass(commandBuffer);

  VkResult result = vkEndCommandBuffer(commandBuffer);
  OGE_ASSERT(result == VK_SUCCESS, "Failed to end Vulkan graphics command buffer.");

  const VkSemaphore waitSemaphores[] = {
    s_rendererState.imageAvailableSemaphores[s_rendererState.currentFrameIndex],
  };

  const VkPipelineStageFlags waitStages[] = {
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
  };

  const VkCommandBuffer commandBuffers[] = {
    s_rendererState.commandBuffers.graphics[s_rendererState.currentFrameIndex],
  };

  const VkSemaphore signalSemaphores[] = {
    s_rendererState.renderFinishedSemaphores[s_rendererState.currentFrameIndex]
  };

  const VkSubmitInfo submitInfo = {
    .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext                = 0,
    .waitSemaphoreCount   = 1,
    .pWaitSemaphores      = waitSemaphores,
    .pWaitDstStageMask    = waitStages,
    .commandBufferCount   = 1,
    .pCommandBuffers      = commandBuffers,
    .signalSemaphoreCount = 1,
    .pSignalSemaphores    = signalSemaphores,
  };

  result = vkQueueSubmit(
    s_rendererState.queues.graphics, 1, &submitInfo,
    s_rendererState.inFlightFences[s_rendererState.currentFrameIndex]);
  OGE_ASSERT(result == VK_SUCCESS, "Failed to submit Vulkan graphics command buffer.");

  const VkPresentInfoKHR presentInfo = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pNext = 0,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = signalSemaphores,
    .swapchainCount = 1,
    .pSwapchains = &s_rendererState.swapchain,
    .pImageIndices = &s_rendererState.currentImageIndex,
  };

  result = vkQueuePresentKHR(s_rendererState.queues.present, &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR ||
      result == VK_SUBOPTIMAL_KHR /*|| m_frameBufferResized */)
  {
    // m_frameBufferResized = false;
    // recreateSwapchain();
    OGE_WARN("Swapchain recreation required.");
  }
  OGE_ASSERT(result == VK_SUCCESS, "Failed to present Vulkan image.");

  s_rendererState.currentFrameIndex = 
    (s_rendererState.currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void ogeRendererWaitIdle() {
  vkDeviceWaitIdle(s_rendererState.logicalDevice);
}
