#pragma once

#include "oge/defines.h"
#include "oge/renderer/renderer.h"

typedef struct OgeRendererState {
  b8 initialized;

  b8   (*pInitFunction)(OgeRendererInitInfo *pInitInfo);
  void (*pTerminateFunction)();
} OgeRendererState;
