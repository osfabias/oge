#include "oge/core/input.h"
#include "oge/core/engine.h"
#include "oge/core/events.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/assertion.h"
#include "oge/core/application.h"
#include "oge/renderer/renderer.h"

struct {
  b8 initialized;
  const OgeApplication *application;
} s_ogeState = { .initialized = OGE_FALSE };

OGE_INLINE b8 initSystems() {
  const OgeInitInfo initInfo = *s_ogeState.application->ogeInitInfo;

  if (!ogeLoggingInit(initInfo.loggingInitInfo)) {
    OGE_ERROR("Failed to initizlize logging system.");
  }

  if (!ogePlatformInit(initInfo.platformInitInfo)) {
    OGE_ERROR("Failed to initialize platform layer.");
    return OGE_FALSE;
  }
  OGE_INFO("OPL initialized.");

  ogeEventsInit();
  ogeInputInit();

  if (!ogeRendererInit(initInfo.rendererInitInfo)) {
    OGE_ERROR("Failed to initialize renderer.");
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

b8 ogeInit(const OgeApplication *application) {
  OGE_ASSERT(
    !s_ogeState.initialized,
    "Trying to initialize OGE while it's already initialized."
  );

  OGE_INFO("OGE info\nversion: %d.%d.%d",
           OGE_VERSION_MAJOR,
           OGE_VERSION_MINOR,
           OGE_VERSION_PATCH);

  s_ogeState.application = application;

  if (!initSystems()) { return OGE_FALSE; }
  
  if(!s_ogeState.application->init()) {
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
  while (!ogePlatformAppShouldClose()) {
    ogePlatformPumpMessages();

    if (!s_ogeState.application->update()) {
      OGE_ERROR("Failed on OGE application update function call.");
      break;
    }

    if (!s_ogeState.application->render()) {
      OGE_ERROR("Failed on OGE application render function call.");
      break;
    }

    ogeInputUpdate();
  }
  OGE_INFO("Quitting main cycle.");
}

void ogeTerminate() {
  OGE_ASSERT(
    s_ogeState.initialized,
    "Trying to initialize OGE while it's already initialized."
  );

  s_ogeState.application->terminate();
  OGE_INFO("OGE application terminated.");

  terminateSystems();

  OGE_INFO("OGE terminated.");
}
