#include "oge/oge.h"

// OGE configuration
OgeLoggingInitInfo loggingInitInfo = {
  .logLevel  = OGE_LOG_LEVEL_TRACE,
  .pFileName = "logs.txt",
};

OplInitInfo oplInitInfo = {
  .pApplicationName = "OGE application",
  .surfaceWidth     = 1280,
  .surfaceHeight    = 720,
};

OgeRendererInitInfo rendererInitInfo = {
  .applicationVersion = OGE_MAKE_VERSION(0, 0, 1),
  .pApplicationName = "OGE application",
};

OgeInitInfo ogeInitInfo = {
  .pLoggingInitInfo  = &loggingInitInfo,
  .pOplInitInfo = &oplInitInfo,
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
  pApplication->pOgeInitInfo = &ogeInitInfo;
  pApplication->pfnInit      = applicationInit;
  pApplication->pfnUpdate    = applicationUpdate;
  pApplication->pfnRender    = applicationRender;
  pApplication->pfnTerminate = applicationTerminate;

  return OGE_TRUE;
}
