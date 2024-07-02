#include "oge/core/engine.h"
#include "oge/core/events.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/application.h"

struct {
  b8 initialized;
  const OgeApplication *pApplication;
} ogeState = { .initialized = OGE_FALSE };

OGE_INLINE b8 initSystems() {
  if(!ogeLoggingInit(ogeState.pApplication->pOgeInitInfo->pLoggingInitInfo)) {
    return OGE_FALSE;
  }

  if (!ogePlatformInit(ogeState.pApplication->pOgeInitInfo->pPlatformInitInfo)) {
    return OGE_FALSE;
  }

  ogeEventsInit();

  return OGE_TRUE;
}

OGE_INLINE void terminateSystems() {
  ogeEventsTerminate();
  ogePlatformTerminate();
  ogeLoggingTerminate();
}

b8 ogeInit(const OgeApplication *pApplication) {
  if (ogeState.initialized) {
    OGE_WARN("Trying to initialize OGE while it's already initialized.");
    return OGE_TRUE;
  }

  ogeState.pApplication = pApplication;

  if (!initSystems()) { return OGE_FALSE; }
  
  // Initialize application
  if(!ogeState.pApplication->pInitFunction(ogeState.pApplication)) {
    OGE_FATAL("Failed to init OGE application.");
    return 1;
  }
  OGE_INFO("OGE application initialized.");

  ogeState.initialized = OGE_TRUE;

  OGE_INFO("OGE initialized.");
  return OGE_TRUE;
}

void ogeRun() {
  OGE_INFO("Entering main cycle.");
  while (!ogePlatformTerminateRequested()) {
    ogePlatformPumpMessages();

    if (!ogeState.pApplication->pUpdateFunction(ogeState.pApplication)) {
      OGE_ERROR("Failed on OGE application update function call.");
      break;
    }

    if (!ogeState.pApplication->pRenderFunction(ogeState.pApplication)) {
      OGE_ERROR("Failed on OGE application render function call.");
      break;
    }
  }
  OGE_INFO("Quitting main cycle.");
}

void ogeTerminate() {
  if (!ogeState.initialized) {
    OGE_WARN("Trying to terminate OGE while it's already terminated.");
    return;
  }

  ogeState.pApplication->pTerminateFunction(ogeState.pApplication);
  OGE_INFO("OGE application terminated.");

  terminateSystems();

  OGE_INFO("OGE terminated.");
}
