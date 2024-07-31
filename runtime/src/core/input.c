#include <opl/opl.h>

#include "oge/defines.h"
#include "oge/core/input.h"
#include "oge/core/memory.h"
#include "oge/core/logging.h"
#include "oge/core/assertion.h"

struct {
  b8                      initialized;
  const OplKeyboardState *keyboardStateCurrent;
  OplKeyboardState        keyboardStatePrevious;
  const OplMouseState    *mouseStateCurrent;
  OplMouseState           mouseStatePrevious;
} s_inputState = { .initialized = OGE_FALSE };

void ogeInputInit() {
  OGE_ASSERT(
    !s_inputState.initialized,
    "Trying to initialize input system while it's already initialized."
  );

  ogeMemSet(&s_inputState, 0, sizeof(s_inputState));
  s_inputState.initialized = OGE_TRUE;
  s_inputState.keyboardStateCurrent = oplKeyboardGetState();
  s_inputState.mouseStateCurrent = oplMouseGetState();

  OGE_INFO("Input system initialized.");
}

void ogeInputTerminate() {
  OGE_ASSERT(
    s_inputState.initialized,
    "Trying to terminate input system while it's already terminated."
  );

  s_inputState.initialized = OGE_FALSE;

  OGE_INFO("Input system terminated.");
}

void ogeInputUpdate() {
  // copy current states to previous
  ogeMemCpy(&s_inputState.keyboardStatePrevious,
            s_inputState.keyboardStateCurrent,
            sizeof(OplKeyboardState));

  ogeMemCpy(&s_inputState.mouseStatePrevious,
            s_inputState.mouseStateCurrent,
            sizeof(s_inputState.mouseStatePrevious));
}

OGE_API b8 ogeKeyIsPressed(OplKey key) {
  return s_inputState.keyboardStateCurrent->keys[key] &&
         !s_inputState.keyboardStatePrevious.keys[key];
}

OGE_API b8 ogeKeyIsDown(OplKey key) {
  return s_inputState.keyboardStateCurrent->keys[key];
}

OGE_API b8 ogeKeyIsReleased(OplKey key) {
  return !s_inputState.keyboardStateCurrent->keys[key] &&
         s_inputState.keyboardStatePrevious.keys[key];
}

OGE_API b8 ogeKeyIsUp(OplKey key) {
  return !s_inputState.keyboardStateCurrent->keys[key];
}

OGE_API b8 ogeMouseButtonIsPressed(OplMouseButton mouseButton) {
  return s_inputState.mouseStateCurrent->buttons[mouseButton] &&
         !s_inputState.mouseStatePrevious.buttons[mouseButton];
}

OGE_API b8 ogeMouseButtonIsDown(OplMouseButton mouseButton) {
  return s_inputState.mouseStateCurrent->buttons[mouseButton];
}

OGE_API b8 ogeMouseButtonIsReleased(OplMouseButton mouseButton) {
  return !s_inputState.mouseStateCurrent->buttons[mouseButton] &&
         s_inputState.mouseStatePrevious.buttons[mouseButton];
}

OGE_API b8 ogeMouseButtonIsUp(OplMouseButton mouseButton) {
  return !s_inputState.mouseStateCurrent->buttons[mouseButton];
}

OGE_API i8 ogeMouseGetWheel() {
  return s_inputState.mouseStateCurrent->wheel;
}

OGE_API void ogeCursorGetPosition(u16 *x, u16 *y) {
  *x = s_inputState.mouseStateCurrent->x;
  *y = s_inputState.mouseStateCurrent->y;
}

OGE_API void ogeCursorGetDelta(u16 *x, u16 *y) {
  *x = s_inputState.mouseStatePrevious.x -
       s_inputState.mouseStateCurrent->x;
  *y = s_inputState.mouseStatePrevious.y -
       s_inputState.mouseStateCurrent->y;
}

