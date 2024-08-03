#include "oge/oge.h"

// OGE configuration
const OgeLoggingInitInfo loggingInitInfo = {
  .logLevel  = OGE_LOG_LEVEL_TRACE,
  .fileName  = "logs.txt",
};

const OgePlatformInitInfo platformInitInfo = {
  .applicationName = "OGE application",
  .width           = 640,
  .height          = 360,
};

const OgeRendererInitInfo rendererInitInfo = {
  .applicationVersion     = OGE_MAKE_VERSION(0, 0, 1),
  .applicationName        = "OGE application",
  .vertexShaderFileName   = "shaders/spir-v/main-vert.spv",
  .fragmentShaderFileName = "shaders/spir-v/main-frag.spv",
  .clearColor             = { 0.0f, 0.8f, 0.3f, 1.0f }
};

const OgeInitInfo ogeInitInfo = {
  .loggingInitInfo  = &loggingInitInfo,
  .platformInitInfo = &platformInitInfo,
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
  ogeRendererStartScene();
  ogeRendererEndScene();
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
