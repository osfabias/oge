#pragma once

#include "opl/opl.h"

#include "oge/defines.h"

//for internal use only
#define _KEY(name, code) OGE_KEYBOARD_KEY_##name = code


void ogeInputInit();
void ogeInputTerminate();

void ogeInputUpdate();

OGE_API b8 ogeInputIsKeyPressed(OplKey key);
OGE_API b8 ogeInputIsKeyDown(OplKey key);
OGE_API b8 ogeInputIsKeyReleased(OplKey key);
OGE_API b8 ogeInputIsKeyUp(OplKey key);

OGE_API b8 ogeInputIsMouseButtonPressed(OplMouseButton mouseButton);
OGE_API b8 ogeInputIsMouseButtonDown(OplMouseButton mouseButton);
OGE_API b8 ogeInputIsMouseButtonReleased(OplMouseButton mouseButton);
OGE_API b8 ogeInputIsMouseButtonUp(OplMouseButton mouseButton);
OGE_API i8 ogeInputGetMouseWheel();
OGE_API void ogeInputGetMousePosition(u16 *x, u16 *y);
OGE_API void ogeInputGetMouseDelta(u16 *x, u16 *y);
