#include "oge/core/input.h"
#include "oge/core/engine.h"
#include "oge/core/events.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/application.h"

struct {
  b8 initialized;
  const OgeApplication *pApplication;
} s_ogeState = { .initialized = OGE_FALSE };

OGE_INLINE b8 initSystems() {
  if(!ogeLoggingInit(s_ogeState.pApplication->pOgeInitInfo->pLoggingInitInfo)) {
    return OGE_FALSE;
  }

  if (!ogePlatformInit(s_ogeState.pApplication->pOgeInitInfo->pPlatformInitInfo)) {
    return OGE_FALSE;
  }

  ogeEventsInit();
  ogeInputInit();

  return OGE_TRUE;
}

OGE_INLINE void terminateSystems() {
  ogeInputTerminate();
  ogeEventsTerminate();
  ogePlatformTerminate();
  ogeLoggingTerminate();
}

b8 ogeInit(const OgeApplication *pApplication) {
  if (s_ogeState.initialized) {
    OGE_WARN("Trying to initialize OGE while it's already initialized.");
    return OGE_TRUE;
  }

  s_ogeState.pApplication = pApplication;

  if (!initSystems()) { return OGE_FALSE; }
  
  // Initialize application
  if(!s_ogeState.pApplication->pInitFunction(s_ogeState.pApplication)) {
    OGE_FATAL("Failed to init OGE application.");
    return 1;
  }
  OGE_INFO("OGE application initialized.");

  s_ogeState.initialized = OGE_TRUE;

  OGE_INFO("OGE initialized.");
  return OGE_TRUE;
}

void ogeRun() {
  OGE_INFO("Entering main cycle.");
  while (!ogePlatformTerminateRequested()) {
    ogePlatformPumpMessages();

    if (!s_ogeState.pApplication->pUpdateFunction(s_ogeState.pApplication)) {
      OGE_ERROR("Failed on OGE application update function call.");
      break;
    }

    if (!s_ogeState.pApplication->pRenderFunction(s_ogeState.pApplication)) {
      OGE_ERROR("Failed on OGE application render function call.");
      break;
    }

    ogeInputUpdate();
  }
  OGE_INFO("Quitting main cycle.");
}

void ogeTerminate() {
  if (!s_ogeState.initialized) {
    OGE_WARN("Trying to terminate OGE while it's already terminated.");
    return;
  }

  s_ogeState.pApplication->pTerminateFunction(s_ogeState.pApplication);
  OGE_INFO("OGE application terminated.");

  terminateSystems();

  OGE_INFO("OGE terminated.");
}
