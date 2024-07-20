#include "oge/core/input.h"
#include "oge/core/engine.h"
#include "oge/core/events.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/application.h"
#include "oge/renderer/renderer.h"

struct {
  b8 initialized;
  const OgeApplication *pApplication;
} s_ogeState = { .initialized = OGE_FALSE };

OGE_INLINE b8 initSystems() {
  const OgeInitInfo initInfo = *s_ogeState.pApplication->pOgeInitInfo;

  if(!ogeLoggingInit(initInfo.pLoggingInitInfo)) {
    return OGE_FALSE;
  }

  if (!ogePlatformInit(initInfo.pPlatformInitInfo)) {
    return OGE_FALSE;
  }

  ogeEventsInit();
  ogeInputInit();

  if (!ogeRendererInit(initInfo.pRendererInitInfo)) {
    return OGE_FALSE;
  }

  return OGE_TRUE;
}

OGE_INLINE void terminateSystems() {
  ogeRendererTerminate();
  ogeInputTerminate();
  ogeEventsTerminate();
  ogePlatformTerminate();
  ogeLoggingTerminate();
}

b8 ogeInit(const OgeApplication *pApplication) {
  OGE_INFO("OGE info\nversion: %d.%d.%d", OGE_VERSION_MAJOR, OGE_VERSION_MINOR, OGE_VERSION_PATCH);

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
