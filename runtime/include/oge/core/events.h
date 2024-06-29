#pragma once

#include "oge/defines.h"

typedef struct OgeEventData {
  // 128 bytes
  union {
    u64 u64[2];
    i64 i64[2];
    f64 f64[2];

    u32 u32[4];
    i32 i32[4];
    f32 f32[4];

    u16 u16[8];
    i16 i16[8];

    u8  u8[16];
    i8  i8[16];

    char c[16];
  };
} OgeEventData;

typedef void(OgeEventCallback)(void *sender, OgeEventData data);


b8   ogeEventsInit();
void ogeEventsTerminate();

OGE_API void ogeEventsSetCallback(u16 code, OgeEventCallback pCallback);
OGE_API void ogeEventsInvoke(u16 code, void *sender, OgeEventData data);

// OGE reserved event codes (up to 255)
typedef enum OgeEventCode {
  OGE_EVENT_UNKOWN,

  // Application (platform) events
  OGE_EVENT_APPLICATION_QUIT,
  OGE_EVENT_APPLICATION_MINIMIZE,
  OGE_EVENT_APPLICATION_MAXIMIZE,

  // Keyboard events
  OGE_EVENT_KEY_PRESS,
  OGE_EVENT_KEY_RELEASE,
  OGE_EVENT_KEY_REPEAT,

  // Mouse events
  OGE_EVENT_MOUSE_PRESS,
  OGE_EVENT_MOUSE_RELEASE,
  OGE_EVENT_MOUSE_WHEEL,
  OGE_EVENT_MOUSE_MOVE,

  OGE_EVENT_MAX_ENUM = 255,
} OgeEventCode;
