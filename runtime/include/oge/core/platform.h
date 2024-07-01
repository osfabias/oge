#pragma once

#include "oge/defines.h"

typedef struct OgePlatformInitInfo {
  const char *pApplicationName;
  u16 width;
  u16 height;
} OgePlatformInitInfo;

b8    ogePlatformInit(const OgePlatformInitInfo *pInitInfo);

void  ogePlatformTerminate();

void  ogePlatformPumpMessages();

b8    ogePlatformTerminateRequested();

void* ogePlatformAllocate(u64 size, b8 aligned);

void* ogePlatformReallocate(void *pBlock, u64 size, b8 aligned);

void  ogePlatformDeallocate(void *pBlock, b8 aligned);

void  ogePlatformMemoryCopy(void *pDstBlock, const void *pSrcBlock, u64 size);

void  ogePlatformMemorySet(void *pDstBlock, i32 value, u64 size);

void ogePlatformMemoryMove(void *pDstBlock, const void *pSrcBlock, u64 size);

void  ogePlatformConsoleWrite(const char *pMessage, u8 color);

/**
 * @brief Returns amount of seconds past since the unix epoch.
 */
f64 ogePlatformGetTime();

/**
 * @brief Sleep on current thread for the given amount of milliseconds.
 *
 * Should only be used for giving time back to the OS for unused
 * update power in order not to use 100% of cpu.
 *
 */
void ogePlatformSleep(u64 ms);
