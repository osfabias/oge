#include <stdio.h>
#include <string.h>

#include <vulkan/vulkan.h>

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

// TODO: write custom allocators for vulkan
struct {
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

  VkPhysicalDevice physicalDevice;
  VkPhysicalDeviceFeatures physicalDeviceFeatures;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
  queueFamilyIndicies queueFamilyIndicies;

  VkDevice logicalDevice;
  VkQueue graphicsQueue;
  VkQueue transferQueue;
  VkQueue computeQueue;
  VkQueue presentQueue;

  VkRenderPass renderPass;

  VkPipeline pipeline;
  VkPipelineLayout pipelineLayout;

  VkAllocationCallbacks *pAllocator;
  #ifdef OGE_DEBUG
  VkDebugUtilsMessengerEXT debugMessenger;
  #endif
} s_rendererState = {
  .initialized = OGE_FALSE,

  .pAllocator = 0, // temporary, while custom allocator isn't written
};

const char *s_ppRequiredDeviceExtensions[] = {
#ifdef OGE_PLATFORM_APPLE
  #define REQUIRED_DEVICE_EXTENSIONS_COUNT 2

  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  "VK_KHR_portability_subset",
#elif OGE_PLATFORM_LINUX
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
      "Selected GPU:\nname: %s\ndriver version: %d.%d.%d\nVulkan API version: %d.%d.%d", 
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
    0, &s_rendererState.graphicsQueue);

  vkGetDeviceQueue(
    s_rendererState.logicalDevice,
    s_rendererState.queueFamilyIndicies.transfer,
    0, &s_rendererState.transferQueue);

  vkGetDeviceQueue(
    s_rendererState.logicalDevice,
    s_rendererState.queueFamilyIndicies.compute,
    0, &s_rendererState.computeQueue);

  vkGetDeviceQueue(
    s_rendererState.logicalDevice,
    s_rendererState.queueFamilyIndicies.present,
    0, &s_rendererState.presentQueue);
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
                           &s_rendererState.pipelineLayout);
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
    .layout              = s_rendererState.pipelineLayout,
    .renderPass          = s_rendererState.renderPass,
    .subpass             = 0,
    .basePipelineHandle  = VK_NULL_HANDLE,
  };

  const VkResult result =
    vkCreateGraphicsPipelines(s_rendererState.logicalDevice,
                              VK_NULL_HANDLE, 1,
                              &pipelineInfo,
                              s_rendererState.pAllocator,
                              &s_rendererState.pipeline);

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

b8 ogeRendererInit(const OgeRendererInitInfo *pInitInfo) {
  OGE_ASSERT(
    !s_rendererState.initialized,
    "Trying to initialize renderer while it's already initialized."
  );

  OGE_WARN("Vulkan uses default allocator, write custom one.");

  OGE_TRACE("Initializing renderer.");

  if (!createInstance(pInitInfo)) { return OGE_FALSE; }

  #ifdef OGE_DEBUG
  createDebugMessenger();
  #endif

  if (!createSurface())                   { return OGE_FALSE; }
  if (!selectGPU())                       { return OGE_FALSE; }
  if (!createLogicalDevice())             { return OGE_FALSE; }

  getQueues();

  if (!createSwapchain())                 { return OGE_FALSE; }

  getSwapchainImages();

  if (!createSwapchainImageViews())       { return OGE_FALSE; }
  if (!createRenderPass())                { return OGE_FALSE; }
  if (!createGraphicsPipelineLayout())    { return OGE_FALSE; }
  if (!createGraphicsPipeline(pInitInfo)) { return OGE_FALSE; }

  s_rendererState.initialized = OGE_TRUE;
  OGE_TRACE("Renderer initialized.");
  return OGE_TRUE;
}

void ogeRendererTerminate() {
  OGE_ASSERT(
    s_rendererState.initialized,
    "Trying to terminate renderer while it's already terminated."
  );

  OGE_TRACE("Terminating Vulkan renderer.");


  vkDestroyPipeline(s_rendererState.logicalDevice,
                    s_rendererState.pipeline,
                    s_rendererState.pAllocator);

  vkDestroyPipelineLayout(s_rendererState.logicalDevice,
                          s_rendererState.pipelineLayout,
                          s_rendererState.pAllocator);

  vkDestroyRenderPass(s_rendererState.logicalDevice,
                      s_rendererState.renderPass,
                      s_rendererState.pAllocator);

  for (u32 i = 0; i < s_rendererState.swapchainImageCount; ++i) {
    vkDestroyImageView(s_rendererState.logicalDevice,
                       s_rendererState.swapchainImageViews[i], 
                       s_rendererState.pAllocator);
  }

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
