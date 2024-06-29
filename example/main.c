#include "oge/containers/darray.h"
#include "oge/core/memory.h"
#include "oge/oge.h"

// OGE configuration
OgeLoggingInitInfo loggingInitInfo = {
  .logLevel  = OGE_LOG_LEVEL_TRACE,
  .pFileName = "logs.txt",
};

OgePlatformInitInfo platformInitInfo = {
  .pApplicationName = "OGE application",
  .width            = 1280,
  .height           = 720,
};

OgeInitInfo ogeInitInfo = {
  .pLoggingInitInfo  = &loggingInitInfo,
  .pPlatformInitInfo = &platformInitInfo,
};

// Application functions
b8 applicationInit(const OgeApplication *pApplication) {
  u16 *pDArray = ogeDArrayAllocate(112, sizeof(u16));

  OGE_WARN(ogeMemoryGetDebugInfo());

  pDArray = ogeDArrayResize(pDArray, 190);

  OGE_WARN(ogeMemoryGetDebugInfo());

  pDArray = ogeDArrayShrink(pDArray);

  OGE_WARN(ogeMemoryGetDebugInfo());

  ogeDArrayDeallocate(pDArray);

  OGE_WARN(ogeMemoryGetDebugInfo());

  return OGE_TRUE;
}

b8 applicationUpdate(const OgeApplication *pApplication) {
  return OGE_TRUE;
}

b8 applicationRender(const OgeApplication *pApplication) {
  return OGE_TRUE;
}

void applicationTerminate(const OgeApplication *pApplication) { }

// Application create function
b8 ogeApplicationCreate(OgeApplication *pApplication) {
  pApplication->pOgeInitInfo       = &ogeInitInfo;
  pApplication->pInitFunction      = applicationInit;
  pApplication->pUpdateFunction    = applicationUpdate;
  pApplication->pRenderFunction    = applicationRender;
  pApplication->pTerminateFunction = applicationTerminate;

  return OGE_TRUE;
}
