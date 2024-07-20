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

OgeRendererInitInfo rendererInitInfo = {
  .graphicsAPI = OGE_GRAPHICS_API_VULKAN,
  .applicationVersion = OGE_MAKE_VERSION(0, 0, 0),
};

OgeInitInfo ogeInitInfo = {
  .pLoggingInitInfo  = &loggingInitInfo,
  .pPlatformInitInfo = &platformInitInfo,
  .pRendererInitInfo = &rendererInitInfo,
};

// Application functions
b8 applicationInit(const OgeApplication *pApplication) {
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
