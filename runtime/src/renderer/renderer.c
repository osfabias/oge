#include "oge/defines.h"
#include "oge/core/logging.h"
#include "oge/renderer/metal.h"
#include "oge/renderer/vulkan.h"
#include "oge/renderer/directx.h"
#include "oge/renderer/renderer.h"
#include "oge/renderer/renderer-state.h"

OgeRendererState s_rendererState = { .initialized = OGE_FALSE };

b8 ogeRendererInit(OgeRendererInitInfo *pInitInfo) {
  if (s_rendererState.initialized) {
    OGE_WARN("Trying to initialize renderer while it's already initialized.");
    return OGE_TRUE;
  }

  switch (pInitInfo->graphicsAPI) {
    case OGE_GRAPHICS_API_VULKAN:
      #ifdef OGE_VULKAN_ENABLED
      ogeVulkanCreate(&s_rendererState);
      #else
      OGE_ERROR("Trying to initialize renderer with Vulkan, but Vulkan isn't supported on this platform.");
      #endif
      break;
    case OGE_GRAPHICS_API_METAL:
      #ifdef OGE_METAL_ENABLED
      ogeMetalCreate(&s_rendererState);
      #else
      OGE_ERROR("Trying to initialize renderer with Metal, but Metal isn't supported on this platform.");
      #endif
      break;
    case OGE_GRAPHICS_API_DIRECTX:
      #ifdef OGE_DIRECTX_ENABLED
      ogeDirectXCreate(&s_rendererState);
      #else
      OGE_ERROR("Trying to initialize renderer with DirectX, but DirectX isn't supported on this platform.");
      #endif
      break;
    default:
      OGE_ERROR("Trying to initialize renderer passing unknown graphics API: %d", pInitInfo->graphicsAPI);
      return OGE_FALSE;
  }

  if (s_rendererState.pInitFunction(pInitInfo)) {
    OGE_INFO("Renderer initialized.");
    s_rendererState.initialized = OGE_TRUE;
    return OGE_TRUE;
  }

  OGE_ERROR("Failed to initilize renderer.");
  return OGE_FALSE;
}

void ogeRendererTerminate() {
  if (!s_rendererState.initialized) {
    OGE_WARN("Trying to terminate renderer while it's already terminated.");
    return;
  }

  s_rendererState.pTerminateFunction();
  s_rendererState.initialized = OGE_FALSE;

  OGE_INFO("Renderer terminated.");
}
