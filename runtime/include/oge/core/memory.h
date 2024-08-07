/**
 * @file memory.h
 * @brief The header of the memory system 
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

#include <opl/opl.h>

#include "oge/defines.h"

/**
 * @brief Memory tag.
 *
 * Memory tags are used in debug builds to track memory usage
 * per tag.
 */
typedef enum OgeMemoryTag {
  OGE_MEMORY_TAG_UNKNOWN,

  OGE_MEMORY_TAG_ARRAY,
  OGE_MEMORY_TAG_DARRAY,
  OGE_MEMORY_TAG_DICT,
  OGE_MEMORY_TAG_STRING,
  OGE_MEMORY_TAG_APPLICATION,
  OGE_MEMORY_TAG_TEXTURE,
  OGE_MEMORY_TAG_MATERIAL,
  OGE_MEMORY_TAG_RENDERER,
  OGE_MEMORY_TAG_GAME,
  OGE_MEMORY_TAG_TRANSFORM,
  OGE_MEMORY_TAG_ENTITY,
  OGE_MEMORY_TAG_ENTITY_NODE,
  OGE_MEMORY_TAG_SCENE,

  OGE_MEMORY_TAG_MAX_ENUM
} OgeMemoryTag;

/**
 * @brief Initialized memory system.
 *
 * Should be called before any OGE function is called.
 */
OGE_API void ogeMemoryInit();

/**
 * @brief Terminates memory system.
 */
OGE_API void ogeMemoryTerminate();

/**
 * @brief Allocates a block of memory.
 * @param size A size of block in bytes.
 * @param memoryTag A memory tag.
 * @return Returns a pointer to an allocated memory block.
 */
OGE_API void* ogeAlloc(u64 size, OgeMemoryTag memoryTag);

/**
 * @brief Reallocates a block of memory.
 * @param block A pointer to a block of memory.
 * @param size A new size of block in bytes.
 * @return Returns a pointer to a reallocated memory block.
 */
OGE_API void* ogeRealloc(void *block, u64 size);

/**
 * @brief Frees a block of memory.
 * @param block A pointer to a block of memory.
 */
OGE_API void ogeFree(void *block);

/**
 * @brief Copies a block of memory.
 * @param dst A pointer to a memory block to copy to.
 * @param str A pointer to a memory block to copy from.
 * @param size A size of memory to copy in bytes.
 */
OGE_API void ogeMemCpy(void *dst, const void *src, u64 size);

/**
 * @brief Sets a block of memory to a value.
 * @param block A pointer to a memory block.
 * @param value A value to set a memory block to.
 * @param size A size of the memory block to set value to.
 */
OGE_API void ogeMemSet(void *block, i32 value, u64 size);

/**
 * @brief Moves a block of memory.
 * @param dst A pointer to a block of memory to move from.
 * @param src A pointer to a block of memory to move to.
 * @param size A size of the memory block to move in bytes.
 */
OGE_API void ogeMemMove(void *dst, const void *src, u64 size);

/**
 * @brief Compares two blocks of memory.
 * @param 
 * @return Retuns 0 if block are equal, otherwise returns a
 *         non-zero value.
 */
OGE_API i32 ogeMemCmp(const void *block1, const void *block2, u64 size);

/**
 * @brief Returns a debug info.
 *
 * In release builds returns an empty string.
 */
OGE_API const char* ogeMemoryGetDebugInfo();

/**
 * @brief Returns a string representation of a memory tag.
 *
 * In release builds always returns an empty string.
 *
 * @param memoryTag A memory tag.
 */
OGE_API const char* ogeMemoryTagToString(OgeMemoryTag memoryTag);

