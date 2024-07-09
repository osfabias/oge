#pragma once

#include "oge/defines.h"

//for internal use only
#define _KEY(name, code) OGE_KEYBOARD_KEY_##name = code

typedef enum OgeMouseButton {
  OGE_MOUSE_BUTTON_LEFT,
  OGE_MOUSE_BUTTON_RIGHT,
  OGE_MOUSE_BUTTON_MIDDLE,
  OGE_MOUSE_BUTTON_FORWARD,
  OGE_MOUSE_BUTTON_BACK,

  OGE_MOUSE_BUTTON_MAX_ENUM
} OgeMouseButton;

typedef enum OgeKey {
  OGE_KEY_BACKSPACE = 0x08,
  OGE_KEY_ENTER = 0x0D,
  OGE_KEY_TAB = 0x09,
  OGE_KEY_SHIFT = 0x10,
  OGE_KEY_CONTROL = 0x11,

  OGE_KEY_PAUSE = 0x13,
  OGE_KEY_CAPSLOCK = 0x14,

  OGE_KEY_ESCAPE = 0x1B,

  OGE_KEY_CONVERT = 0x1C,
  OGE_KEY_NONCONVERT = 0x1D,
  OGE_KEY_ACCEPT = 0x1E,
  OGE_KEY_MODECHANGE = 0x1F,

  OGE_KEY_SPACE = 0x20,

  OGE_KEY_PAGEUP = 0x21,
  OGE_KEY_PAGEDOWN = 0x22,
  OGE_KEY_END = 0x23,
  OGE_KEY_HOME = 0x24,

  OGE_KEY_LEFT = 0x25,
  OGE_KEY_UP = 0x26,
  OGE_KEY_RIGHT = 0x27,
  OGE_KEY_DOWN = 0x28,

  OGE_KEY_SELECT = 0x29,
  OGE_KEY_PRINT = 0x2A,
  OGE_KEY_EXECUTE = 0x2B,
  OGE_KEY_PRINTSCREEN = 0x2C,
  OGE_KEY_INSERT = 0x2D,
  OGE_KEY_DELETE = 0x2E,
  OGE_KEY_HELP = 0x2F,

  OGE_KEY_0 = 0x30,
  OGE_KEY_1 = 0x31,
  OGE_KEY_2 = 0x32,
  OGE_KEY_3 = 0x33,
  OGE_KEY_4 = 0x34,
  OGE_KEY_5 = 0x35,
  OGE_KEY_6 = 0x36,
  OGE_KEY_7 = 0x37,
  OGE_KEY_8 = 0x38,
  OGE_KEY_9 = 0x39,

  OGE_KEY_A = 0x41,
  OGE_KEY_B = 0x42,
  OGE_KEY_C = 0x43,
  OGE_KEY_D = 0x44,
  OGE_KEY_E = 0x45,
  OGE_KEY_F = 0x46,
  OGE_KEY_G = 0x47,
  OGE_KEY_H = 0x48,
  OGE_KEY_I = 0x49,
  OGE_KEY_J = 0x4A,
  OGE_KEY_K = 0x4B,
  OGE_KEY_L = 0x4C,
  OGE_KEY_M = 0x4D,
  OGE_KEY_N = 0x4E,
  OGE_KEY_O = 0x4F,
  OGE_KEY_P = 0x50,
  OGE_KEY_Q = 0x51,
  OGE_KEY_R = 0x52,
  OGE_KEY_S = 0x53,
  OGE_KEY_T = 0x54,
  OGE_KEY_U = 0x55,
  OGE_KEY_V = 0x56,
  OGE_KEY_W = 0x57,
  OGE_KEY_X = 0x58,
  OGE_KEY_Y = 0x59,
  OGE_KEY_Z = 0x5A,

  OGE_KEY_LSUPER = 0x5B,
  OGE_KEY_RSUPER = 0x5C,

  OGE_KEY_APPS = 0x5D,
  OGE_KEY_SLEEP = 0x5F,

  // by the way, who even uses numpad keys in games?
  OGE_KEY_NUMPAD0 = 0x60,
  OGE_KEY_NUMPAD1 = 0x61,
  OGE_KEY_NUMPAD2 = 0x62,
  OGE_KEY_NUMPAD3 = 0x63,
  OGE_KEY_NUMPAD4 = 0x64,
  OGE_KEY_NUMPAD5 = 0x65,
  OGE_KEY_NUMPAD6 = 0x66,
  OGE_KEY_NUMPAD7 = 0x67,
  OGE_KEY_NUMPAD8 = 0x68,
  OGE_KEY_NUMPAD9 = 0x69,
  OGE_KEY_MULTIPLY = 0x6A,
  OGE_KEY_ADD = 0x6B,
  OGE_KEY_SEPARATOR = 0x6C,
  OGE_KEY_SUBTRACT = 0x6D,
  OGE_KEY_DECIMAL = 0x6E,
  OGE_KEY_DIVIDE = 0x6F,

  OGE_KEY_F1 = 0x70,
  OGE_KEY_F2 = 0x71,
  OGE_KEY_F3 = 0x72,
  OGE_KEY_F4 = 0x73,
  OGE_KEY_F5 = 0x74,
  OGE_KEY_F6 = 0x75,
  OGE_KEY_F7 = 0x76,
  OGE_KEY_F8 = 0x77,
  OGE_KEY_F9 = 0x78,
  OGE_KEY_F10 = 0x79,
  OGE_KEY_F11 = 0x7A,
  OGE_KEY_F12 = 0x7B,
  OGE_KEY_F13 = 0x7C,
  OGE_KEY_F14 = 0x7D,
  OGE_KEY_F15 = 0x7E,
  OGE_KEY_F16 = 0x7F,
  OGE_KEY_F17 = 0x80,
  OGE_KEY_F18 = 0x81,
  OGE_KEY_F19 = 0x82,
  OGE_KEY_F20 = 0x83,
  OGE_KEY_F21 = 0x84,
  OGE_KEY_F22 = 0x85,
  OGE_KEY_F23 = 0x86,
  OGE_KEY_F24 = 0x87,

  OGE_KEY_NUMLOCK = 0x90,
  OGE_KEY_SCROLL = 0x91,
  OGE_KEY_NUMPAD_EQUAL = 0x92,

  OGE_KEY_LSHIFT = 0xA0,
  OGE_KEY_RSHIFT = 0xA1,
  OGE_KEY_LCONTROL = 0xA2,
  OGE_KEY_RCONTROL = 0xA3,
  OGE_KEY_LALT = 0xA4,
  OGE_KEY_RALT = 0xA5,

  OGE_KEY_SEMICOLON = 0x3B,

  OGE_KEY_APOSTROPHE = 0xDE,
  OGE_KEY_EQUAL = 0xBB,
  OGE_KEY_COMMA = 0xBC,
  OGE_KEY_MINUS = 0xBD,
  OGE_KEY_PERIOD = 0xBE,
  OGE_KEY_SLASH = 0xBF,

  OGE_KEY_GRAVE = 0xC0,

  OGE_KEY_LBRACKET = 0xDB,
  OGE_KEY_BACKSLASH = 0xDC,
  OGE_KEY_RBRACKET = 0xDD,

  OGE_KEY_MAX_ENUM = 0xFF
} OgeKey;

void ogeInputInit();
void ogeInputTerminate();

void ogeInputUpdate();

OGE_API b8 ogeInputIsKeyPressed(OgeKey key);
OGE_API b8 ogeInputIsKeyDown(OgeKey key);
OGE_API b8 ogeInputIsKeyReleased(OgeKey key);
OGE_API b8 ogeInputIsKeyUp(OgeKey key);

OGE_API b8 ogeInputIsMouseButtonPressed(OgeMouseButton mouseButton);
OGE_API b8 ogeInputIsMouseButtonDown(OgeMouseButton mouseButton);
OGE_API b8 ogeInputIsMouseButtonReleased(OgeMouseButton mouseButton);
OGE_API b8 ogeInputIsMouseButtonUp(OgeMouseButton mouseButton);
OGE_API i8 ogeInputGetMouseWheel();
OGE_API void ogeInputGetMousePosition(u16 *x, u16 *y);
OGE_API void ogeInputGetMouseDelta(u16 *x, u16 *y);
