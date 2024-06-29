#pragma once

#include "oge/defines.h"

typedef enum OgeMemoryTag {
  OGE_MEMORY_TAG_UNKNOWN,

  OGE_MEMORY_TAG_ARRAY,
  OGE_MEMORY_TAG_DARRAY,
  OGE_MEMORY_TAG_DICT,
  OGE_MEMORY_TAG_RING_QUEUE,
  OGE_MEMORY_TAG_BST,
  OGE_MEMORY_TAG_STRING,
  OGE_MEMORY_TAG_APPLICATION,
  OGE_MEMORY_TAG_JOB,
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

void ogeMemoryInit();
void ogeMemoryTerminate();

OGE_API void* ogeAllocate(u64 size, OgeMemoryTag memoryTag);
OGE_API void  ogeDeallocate(void *block, u64 size, OgeMemoryTag memoryTag);

OGE_API void  ogeMemoryCopy(void *dst, const void *src, u64 size);
OGE_API void  ogeMemorySet(void *block, i32 value, u64 size);

OGE_API const char* ogeMemoryGetDebugInfo();
