/**
 * @file events.h
 * @brief The header of the events system 
 *
 * Copyright (c) 2023-2024 Osfabias
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "oge/defines.h"

/**
 * @brief OGE event data struct.
 *
 * This structure contains data, that's shared between event
 * callbacks.
 */
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

/**
 * @brief Event callback function pointer
 *
 * @param invoker A pointer to event invoker.
 * @param data Event data.
 * @return Returns OGE_TRUE if event was handled and shouldn't 
 *         be dispatched any more, otherwise it returns OGE_FALSE.
 */
typedef b8(*OgeEventCallback)(void *invoker, OgeEventData data);

/**
 * @brief Initialized event system.
 */
void ogeEventsInit();

/**
 * @brief Terminated event system.
 */
void ogeEventsTerminate();

/**
 * @brief Adds a function pointer to a list of an event's callbacks.
 * @param code A code of an event.
 * @param callback A pointer to a function.
 */
OGE_API void ogeEventsSubscribe(u16 code, OgeEventCallback callback);

/**
 * @brief Removes a function pointer from a list of an event's callbacks.
 * @param code A code of an event.
 * @param callback A pointer to a function.
 */
OGE_API void ogeEventsUnsubscribe(u16 code, OgeEventCallback callback);

/**
 * @brief Calls an event's callbacks in subscription order.
 *
 * If a callback function returns OGE_TRUE on call dispatching
 * stops.
 *
 * @param code A code of an event.
 * @param invoker A pointer to an invoker.
 * @param data An event data.
 */
OGE_API void ogeEventsInvoke(u16 code, void *invoker, OgeEventData data);

/**
 * @brief OGE reserved event codes.
 *
 * OGE reserved all event codes up to 255.
 */
typedef enum OgeEventCode {
  OGE_EVENT_UNKOWN,

  OGE_EVENT_APPLICATION_QUIT,
  OGE_EVENT_APPLICATION_MINIMIZE,
  OGE_EVENT_APPLICATION_MAXIMIZE,

  OGE_EVENT_KEY_PRESS,
  OGE_EVENT_KEY_RELEASE,
  OGE_EVENT_KEY_REPEAT,

  OGE_EVENT_MOUSE_BUTTON_PRESS,
  OGE_EVENT_MOUSE_BUTTON_RELEASE,
  OGE_EVENT_MOUSE_WHEEL,
  OGE_EVENT_MOUSE_MOVE,

  OGE_EVENT_MAX_ENUM = 255,
} OgeEventCode;
