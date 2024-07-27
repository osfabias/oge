#include "oge/oge.h"

// OGE configuration
const OgeLoggingInitInfo loggingInitInfo = {
  .logLevel  = OGE_LOG_LEVEL_TRACE,
  .pFileName = "logs.txt",
};

const OplInitInfo oplInitInfo = {
  .pApplicationName = "OGE application",
  .surfaceWidth     = 1280,
  .surfaceHeight    = 720,
};

const OgeRendererInitInfo rendererInitInfo = {
  .applicationVersion = OGE_MAKE_VERSION(0, 0, 1),
  .pApplicationName = "OGE application",
};

const OgeInitInfo ogeInitInfo = {
  .pLoggingInitInfo  = &loggingInitInfo,
  .pOplInitInfo = &oplInitInfo,
  .pRendererInitInfo = &rendererInitInfo,
};

// Application functions
b8 applicationInit(void *pState) {
  return OGE_TRUE;
}

b8 applicationUpdate(void *pState) {
  return OGE_TRUE;
}

b8 applicationRender(void *pState) {
  return OGE_TRUE;
}

void applicationTerminate(void *pState) { }

// Application create function
b8 ogeApplicationCreate(OgeApplication *pApplication) {
  pApplication->pOgeInitInfo = &ogeInitInfo;
  pApplication->pfnInit      = applicationInit;
  pApplication->pfnUpdate    = applicationUpdate;
  pApplication->pfnRender    = applicationRender;
  pApplication->pfnTerminate = applicationTerminate;

  return OGE_TRUE;
}
