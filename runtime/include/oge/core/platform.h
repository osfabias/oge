#pragma once

#include "oge/defines.h"

typedef struct OgePlatformInitInfo {
  const char *pApplicationName;
  u16 width;
  u16 height;
} OgePlatformInitInfo;

/**
 * @brief Initializes platform layer.
 */
b8    ogePlatformInit(const OgePlatformInitInfo *pInitInfo);

/**
 * @brief Terminates platform layer.
 */
void ogePlatformTerminate();

/**
 * @brief Pumps platform messages.
 */
void ogePlatformPumpMessages();

/**
 * @brief Returns OGE_TRUE if platform terminate was requested,
 * otherwise returns OGE_FALSE.
 */
b8 ogePlatformTerminateRequested();

/**
 * @brief Allocates a block of memory of the given size.
 */
void* ogePlatformAllocate(u64 size, b8 aligned);

/**
 * @brief Reallocates a block of memory to the given size.
 */
void* ogePlatformReallocate(void *pBlock, u64 size, b8 aligned);

/**
 * @brief Deallocates a block of memory.
 */
void  ogePlatformDeallocate(void *pBlock, b8 aligned);

/**
 * @bried Copies a block of memory.
 */
void ogePlatformMemoryCopy(void *pDstBlock, const void *pSrcBlock,
                           u64 size);

/**
 * @brief Sets a block of memory to a given value.
 */
void ogePlatformMemorySet(void *pDstBlock, i32 value, u64 size);

/**
 * @brief Moves a block of memory.
 */
void ogePlatformMemoryMove(void *pDstBlock, const void *pSrcBlock,
                           u64 size);

/**
 * @brief Compares two block of memory.
 */
i32 ogePlatformMemoryCompare(const void *pBlock1,
                              const void *pBlock2, u64 size);

/**
 * @brief Writes colorized text to console.
 */
void ogePlatformConsoleWrite(const char *pMessage, u8 color);

/**
 * @brief Returns a pointer to an array of key states.
 *
 * If this function is called on console platforms it
 * returns NULL.
 */
const u8* ogePlatformGetKeyStates();

/**
 * @brief Returns a pointer to an array of mouse button states.
 *
 * If this function is called on console platform it returns
 * NULL.
 */
const u8* ogePlatformGetMouseButtonStates();

/**
 * @brief Returns a mouse position on the screen.
 *
 * If this function is called on console platform it returns
 * zeros.
 */
void ogePlatformGetMousePosition(u16 *x, u16 *y);

/**
 * @brief Returns a mouse wheel delta.
 *
 * It this function is callsed on console platform is returns
 * zero.
 */
i8 ogePlatformGetMouseWheel();

/**
 * @brief Returns amount of milliseconds past since the unix epoch.
 */
u64 ogePlatformGetTime();

/**
 * @brief Sleeps on current thread for the given amount of
 * milliseconds.
 *
 * Should only be used for giving time back to the OS for
 * unused update power in order not to use 100% of cpu.
 */
void ogePlatformSleep(u64 ms);
