#pragma once

#include "oge/internal.h"

typedef struct _OgePlatformInitInfo {
  const char *pApplicationName;
  u16 width;
  u16 height;
} _OgePlatformInitInfo;

b8    _ogePlatformInit(const _OgePlatformInitInfo *pInitInfo);

void  _ogePlatformTerminate();

void  _ogePlatformPumpMessages();

b8    _ogePlatformTerminateRequested();

void* _ogePlatformAllocate(u64 size, b8 aligned);
void  _ogePlatformDeallocate(void *block, b8 aligned);
void  _ogePlatformMemoryCopy(void *dst, const void *src, u64 size);
void  _ogePlatformMemorySet(void *dst, i32 value, u64 size);

void  _ogePlatformConsoleWrite(const char *msg, u8 clr);

/**
 * @brief Returns amount of seconds past since the unix epoch.
 */
u64   _ogePlatformGetTime();

/**
 * @brief Sleep on current thread for the given amount of milliseconds.
 *
 * Should only be used for giving time back to the OS for unused update power.
 */
void  _ogePlatformSleep(u64 ms);
