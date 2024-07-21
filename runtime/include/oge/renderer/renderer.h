#pragma once

#include "oge/defines.h"

typedef struct OgeRendererInitInfo {
  const char *pApplicationName;
  u32 applicationVersion;
} OgeRendererInitInfo;

b8 ogeRendererInit(OgeRendererInitInfo *pInitInfo);
void ogeRendererTerminate();
