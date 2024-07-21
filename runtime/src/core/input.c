#include "oge/defines.h"
#include "oge/core/input.h"
#include "oge/core/events.h"
#include "oge/core/memory.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"

struct {
  b8 initialized;
  const u8* keyStatesCurrent;
  u8  keyStatesPrevious[OGE_KEY_MAX_ENUM];
  struct {
    u16 x;
    u16 y;
    i8 wheel;
    const u8 *buttonStates;
  } mouseStateCurrent;
  struct {
    u16 x;
    u16 y;
    u8 buttonStates[OGE_MOUSE_BUTTON_MAX_ENUM];
  } mouseStatePrevious;
} s_inputState = { .initialized = OGE_FALSE };

void ogeInputInit() {
  if (s_inputState.initialized) {
    OGE_WARN("Trying to initialize input system while it's already initialized.");
    return;
  }

  ogeMemorySet(&s_inputState, 0, sizeof(s_inputState));
  s_inputState.initialized = OGE_TRUE;
  s_inputState.keyStatesCurrent = ogePlatformGetKeyStates();
  s_inputState.mouseStateCurrent.buttonStates =
    ogePlatformGetMouseButtonStates();
}

void ogeInputTerminate() {
  if (!s_inputState.initialized) {
    OGE_WARN("Trying to terminate input system while it's already terminated.");
    return;
  }

  s_inputState.initialized = OGE_FALSE;
}

OGE_INLINE void updateState() {
  // Mouse
  ogePlatformGetMousePosition(&s_inputState.mouseStateCurrent.x,
                              &s_inputState.mouseStateCurrent.y);

  s_inputState.mouseStateCurrent.wheel = ogePlatformGetMouseWheel();
}

OGE_INLINE void invokeEvents() {
  // Key press or release
  for (OgeKey key = 0; key < OGE_KEY_MAX_ENUM; ++key) {
    if (s_inputState.keyStatesCurrent[key] ==
        s_inputState.keyStatesPrevious[key]) { continue; }

    const OgeEventData data = { .u16[0] = key };
    const b8 pressed = s_inputState.keyStatesCurrent[key];
    ogeEventsInvoke(pressed ? OGE_EVENT_KEY_PRESS : OGE_EVENT_KEY_RELEASE,
                    0, data);
  }

  // Mouse button press or release
  for (OgeMouseButton mouseButton = 0;
       mouseButton < OGE_MOUSE_BUTTON_MAX_ENUM;
       ++mouseButton) {
    if (s_inputState.mouseStateCurrent.buttonStates[mouseButton] ==
        s_inputState.mouseStatePrevious.buttonStates[mouseButton]) { continue; }

    const OgeEventData data = { .u16[0] = mouseButton };
    const b8 pressed = s_inputState.mouseStateCurrent.buttonStates[mouseButton];
    ogeEventsInvoke(pressed ? OGE_EVENT_MOUSE_BUTTON_PRESS :
                              OGE_EVENT_MOUSE_BUTTON_RELEASE, 0, data);
  }

  // Mouse wheel
  if (s_inputState.mouseStateCurrent.wheel != 0) {
    const OgeEventData data = { .i8[0] = s_inputState.mouseStateCurrent.wheel };
    ogeEventsInvoke(OGE_EVENT_MOUSE_WHEEL, 0, data);
  }
  
  // Mouse move
  if (s_inputState.mouseStateCurrent.x != s_inputState.mouseStatePrevious.x ||
      s_inputState.mouseStateCurrent.y != s_inputState.mouseStatePrevious.y) {
    const OgeEventData data = {
      .u16[0] = s_inputState.mouseStateCurrent.x,
      .u16[1] = s_inputState.mouseStateCurrent.y,
    };
    ogeEventsInvoke(OGE_EVENT_MOUSE_MOVE, 0, data);
  }
}

void ogeInputUpdate() {
  invokeEvents();

  updateState();

  // copy current states to previous
  ogeMemoryCopy(s_inputState.keyStatesPrevious, s_inputState.keyStatesCurrent,
                sizeof(s_inputState.keyStatesPrevious));

  ogeMemoryCopy(&s_inputState.mouseStatePrevious,
                &s_inputState.mouseStateCurrent,
                sizeof(s_inputState.mouseStatePrevious));
}


OGE_API b8 ogeInputIsKeyPressed(OgeKey key) {
  return s_inputState.keyStatesCurrent[key] &&
         !s_inputState.keyStatesPrevious[key];
}

OGE_API b8 ogeInputIsKeyDown(OgeKey key) {
  return s_inputState.keyStatesCurrent[key];
}

OGE_API b8 ogeInputIsKeyReleased(OgeKey key) {
  return !s_inputState.keyStatesCurrent[key] &&
         s_inputState.keyStatesPrevious[key];
}

OGE_API b8 ogeInputIsKeyUp(OgeKey key) {
  return !s_inputState.keyStatesCurrent[key];
}

OGE_API b8 ogeInputIsMouseButtonPressed(OgeMouseButton mouseButton) {
  return s_inputState.mouseStateCurrent.buttonStates[mouseButton] &&
         !s_inputState.mouseStatePrevious.buttonStates[mouseButton];
}

OGE_API b8 ogeInputIsMouseButtonDown(OgeMouseButton mouseButton) {
  return s_inputState.mouseStateCurrent.buttonStates[mouseButton];
}

OGE_API b8 ogeInputIsMouseButtonReleased(OgeMouseButton mouseButton) {
  return !s_inputState.mouseStateCurrent.buttonStates[mouseButton] &&
         s_inputState.mouseStatePrevious.buttonStates[mouseButton];
}

OGE_API b8 ogeInputIsMouseButtonUp(OgeMouseButton mouseButton) {
  return !s_inputState.mouseStateCurrent.buttonStates[mouseButton];
}

OGE_API i8 ogeInputGetMouseWheel() {
  return s_inputState.mouseStateCurrent.wheel;
}

OGE_API void ogeInputGetMousePosition(u16 *x, u16 *y) {
  *x = s_inputState.mouseStateCurrent.x;
  *y = s_inputState.mouseStateCurrent.y;
}

OGE_API void ogeInputGetMouseDelta(u16 *x, u16 *y) {
  *x = s_inputState.mouseStatePrevious.x - s_inputState.mouseStateCurrent.x;
  *y = s_inputState.mouseStatePrevious.y - s_inputState.mouseStateCurrent.y;
}

