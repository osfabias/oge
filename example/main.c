#include "oge/oge.h"
#include "oge/renderer/renderer.h"

// OGE configuration
const OgeLoggingInitInfo loggingInitInfo = {
  .logLevel  = OGE_LOG_LEVEL_TRACE,
  .fileName  = "logs.txt",
};

const OplInitInfo oplInitInfo = {
  .pApplicationName = "OGE application",
  .surfaceWidth     = 1280,
  .surfaceHeight    = 720,
};

const OgeRendererInitInfo rendererInitInfo = {
  .applicationVersion = OGE_MAKE_VERSION(0, 0, 1),
  .applicationName    = "OGE application",
};

const OgeInitInfo ogeInitInfo = {
  .loggingInitInfo  = &loggingInitInfo,
  .oplInitInfo      = &oplInitInfo,
  .rendererInitInfo = &rendererInitInfo,
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
  pApplication->ogeInitInfo = &ogeInitInfo;
  pApplication->init        = applicationInit;
  pApplication->update      = applicationUpdate;
  pApplication->render      = applicationRender;
  pApplication->terminate   = applicationTerminate;

  return OGE_TRUE;
}
