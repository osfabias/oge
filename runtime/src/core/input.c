#include "opl/opl.h"

#include "oge/defines.h"
#include "oge/core/input.h"
#include "oge/core/events.h"
#include "oge/core/memory.h"
#include "oge/core/logging.h"

struct {
  b8                      initialized;
  const OplKeyboardState *pKeyboardStateCurrent;
  OplKeyboardState        keyboardStatePrevious;
  const OplMouseState    *pMouseStateCurrent;
  OplMouseState           mouseStatePrevious;
} s_inputState = { .initialized = OGE_FALSE };

void ogeInputInit() {
  if (s_inputState.initialized) {
    OGE_WARN("Trying to initialize input system while it's already initialized.");
    return;
  }

  ogeMemSet(&s_inputState, 0, sizeof(s_inputState));
  s_inputState.initialized = OGE_TRUE;
  s_inputState.pKeyboardStateCurrent = oplKeyboardGetState();
  s_inputState.pMouseStateCurrent = oplMouseGetState();

  OGE_INFO("Input system initialized.");
}

void ogeInputTerminate() {
  if (!s_inputState.initialized) {
    OGE_WARN("Trying to terminate input system while it's already terminated.");
    return;
  }

  s_inputState.initialized = OGE_FALSE;

  OGE_INFO("Input system terminated.");
}

void ogeInputUpdate() {
  // copy current states to previous
  ogeMemCpy(&s_inputState.keyboardStatePrevious,
            s_inputState.pKeyboardStateCurrent,
            sizeof(OplKeyboardState));

  ogeMemCpy(&s_inputState.mouseStatePrevious,
            s_inputState.pMouseStateCurrent,
            sizeof(s_inputState.mouseStatePrevious));
}

OGE_API b8 ogeInputIsKeyPressed(OplKey key) {
  return s_inputState.pKeyboardStateCurrent->pKeyStates[key] &&
         !s_inputState.keyboardStatePrevious.pKeyStates[key];
}

OGE_API b8 ogeInputIsKeyDown(OplKey key) {
  return s_inputState.pKeyboardStateCurrent->pKeyStates[key];
}

OGE_API b8 ogeInputIsKeyReleased(OplKey key) {
  return !s_inputState.pKeyboardStateCurrent->pKeyStates[key] &&
         s_inputState.keyboardStatePrevious.pKeyStates[key];
}

OGE_API b8 ogeInputIsKeyUp(OplKey key) {
  return !s_inputState.pKeyboardStateCurrent->pKeyStates[key];
}

OGE_API b8 ogeInputIsMouseButtonPressed(OplMouseButton mouseButton) {
  return s_inputState.pMouseStateCurrent->pButtonStates[mouseButton] &&
         !s_inputState.mouseStatePrevious.pButtonStates[mouseButton];
}

OGE_API b8 ogeInputIsMouseButtonDown(OplMouseButton mouseButton) {
  return s_inputState.pMouseStateCurrent->pButtonStates[mouseButton];
}

OGE_API b8 ogeInputIsMouseButtonReleased(OplMouseButton mouseButton) {
  return !s_inputState.pMouseStateCurrent->pButtonStates[mouseButton] &&
         s_inputState.mouseStatePrevious.pButtonStates[mouseButton];
}

OGE_API b8 ogeInputIsMouseButtonUp(OplMouseButton mouseButton) {
  return !s_inputState.pMouseStateCurrent->pButtonStates[mouseButton];
}

OGE_API i8 ogeInputGetMouseWheel() {
  return s_inputState.pMouseStateCurrent->wheel;
}

OGE_API void ogeInputGetMousePosition(u16 *x, u16 *y) {
  *x = s_inputState.pMouseStateCurrent->x;
  *y = s_inputState.pMouseStateCurrent->y;
}

OGE_API void ogeInputGetMouseDelta(u16 *x, u16 *y) {
  *x = s_inputState.mouseStatePrevious.x -
       s_inputState.pMouseStateCurrent->x;
  *y = s_inputState.mouseStatePrevious.y -
       s_inputState.pMouseStateCurrent->y;
}

