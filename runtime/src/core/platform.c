#include <opl/opl.h>

#include "oge/core/platform.h"
#include "oge/core/assertion.h"
#include "oge/defines.h"

static struct {
  b8 initialized;
  OplWindow *mainWindow;
} s_platformState = { .initialized = OGE_FALSE };

b8 ogePlatformInit(const OgePlatformInitInfo *initInfo) {
  OGE_ASSERT(!s_platformState.initialized, "Trying to initialize platform layer while it's alredy initialized.");

  if (!oplInit()) {
    OGE_ERROR("Failed to initialize OPL.");
    return OGE_FALSE;
  }

  const OplWindowCreateInfo windowCreateInfo = {
    .styleFlags = OPL_WINDOW_STYLE_TITLED        |
                  OPL_WINDOW_STYLE_CLOSABLE      |
                  OPL_WINDOW_STYLE_RESIZABLE     |
                  OPL_WINDOW_STYLE_MINIATURIZABLE,
    .title      = initInfo->applicationName,
    .x          = 0,
    .y          = 0,
    .width      = initInfo->width,
    .height     = initInfo->height,
  };

  s_platformState.mainWindow = oplWindowCreate(&windowCreateInfo);
  if (!s_platformState.mainWindow) {
    OGE_ERROR("Failed to create OPL window.");
    return OGE_FALSE;
  }

  s_platformState.initialized = OGE_TRUE;

  OGE_INFO("Platform layer initialized.");

  #if defined(OGE_PLATFORM_APPLE)
    #define OS_NAME "MacOS "
    #define PLATFORM_NAME "Cocoa"
  #elif defined(OGE_PLATFORM_LINUX)
    #define OS_NAME "Linux "
    #define PLATFORM_NAME "X11"
  #elif defifed(OGE_PLATFORM_WIN32)
    #define OS_NAME "Windows "
    #define PLATFORM_NAME "Win32"
  #endif

  OGE_INFO("Platform layer info:\n\n\t➜ OS:       %s\n\t➜ Platform: %s\n",
           OS_NAME, PLATFORM_NAME);
  return OGE_TRUE;
}

void ogePlatformTerminate() {
  OGE_ASSERT(s_platformState.initialized, "Trying to terminate platform layer while it's already terminated."); 

  oplWindowDestroy(s_platformState.mainWindow);
  oplTerminate();

  s_platformState.initialized = OGE_FALSE;
}

b8 ogePlatformAppShouldClose() {
  return oplWindowShouldClose(s_platformState.mainWindow);
}

VkResult ogePlatformCreateSurface(
  VkInstance instance,
  const VkAllocationCallbacks *allocator,
  VkSurfaceKHR *surface) {

  return oplCreateSurface(s_platformState.mainWindow, instance,
                          allocator, surface);
}
