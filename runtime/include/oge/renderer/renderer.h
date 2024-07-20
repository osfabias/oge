#pragma once

#include "oge/defines.h"

typedef enum OgeGraphicsAPI {
  OGE_GRAPHICS_API_VULKAN,
  OGE_GRAPHICS_API_DIRECTX,
  OGE_GRAPHICS_API_METAL,
} OgeGraphicsAPI;

typedef struct OgeRendererInitInfo {
  OgeGraphicsAPI graphicsAPI;
  const char *pApplicationName;
  u32 applicationVersion;
} OgeRendererInitInfo;

b8 ogeRendererInit(OgeRendererInitInfo *pInitInfo);
void ogeRendererTerminate();
