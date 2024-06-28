#include "oge/core/engine.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/application.h"

struct {
  const OgeApplication *pApplication;
} s_ogeState;

b8 initSystems() {
  if(!ogeLoggingInit(s_ogeState.pApplication->pOgeInitInfo->pLoggingInitInfo)) {
    return OGE_FALSE;
  }

  if (!ogePlatformInit(s_ogeState.pApplication->pOgeInitInfo->pPlatformInitInfo)) {
    OGE_ERROR("Failed to init platform");
    return OGE_FALSE;
  }

  return OGE_TRUE;
}

void terminateSystems() {
  ogePlatformTerminate();
  OGE_INFO("OGE terminated");

  ogeLoggingTerminate();
}

b8 ogeInit(const OgeApplication *pApplication) {
  s_ogeState.pApplication = pApplication;

  if (!initSystems()) { return OGE_FALSE; }
  
  // Initialize application
  if(!s_ogeState.pApplication->pInitFunction(s_ogeState.pApplication)) {
    OGE_FATAL("Failed to init OGE application");
    return 1;
  }
  OGE_INFO("OGE application initialized");

  OGE_INFO("OGE initialized");
  return OGE_TRUE;
}

void ogeRun() {
  OGE_INFO("Entering main cycle");
  while (!ogePlatformTerminateRequested()) {
    ogePlatformPumpMessages();

    if (!s_ogeState.pApplication->pUpdateFunction(s_ogeState.pApplication)) {
      OGE_ERROR("Failed on OGE application update function call");
      break;
    }

    if (!s_ogeState.pApplication->pRenderFunction(s_ogeState.pApplication)) {
      OGE_ERROR("Failed on OGE application render function call");
      break;
    }
  }
  OGE_INFO("Quitting main cycle");
}

void ogeTerminate() {
  s_ogeState.pApplication->pTerminateFunction(s_ogeState.pApplication);
  OGE_INFO("OGE application terminated");
  terminateSystems();
}
