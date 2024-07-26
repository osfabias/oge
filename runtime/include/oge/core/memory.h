#pragma once

#include "opl/opl.h"

#include "oge/defines.h"

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

void ogeMemoryInit();
void ogeMemoryTerminate();

#ifdef OGE_DEBUG
OGE_API void* ogeAlloc(u64 size, OgeMemoryTag memoryTag);
OGE_API void* ogeRealloc(void *pBlock, u64 size);
OGE_API void  ogeFree(void *pBlock);
#else
#define ogeAlloc(size, memorryTag) \
  oplAlloc(size)

#define ogeRealloc(pBlock, size) \
  oplRealloc(pBlock, size)

#define ogeFree(pBlock) \
  oplFree(pBlock)
#endif

#define ogeMemCpy(pDstBlock, pSrcBlock, size) \
  oplMemCpy(pDstBlock, pSrcBlock, size)

#define ogeMemSet(pBlock, value, size) \
  oplMemSet(pBlock, value, size)

#define ogeMemMove(pDstBlock, pSrcBlock, size) \
  oplMemMove(pDstBlock, pSrcBlock, size)

#define ogeMemCmp(pBlock1, pBlock2, size) \
  oplMemCmp(pBlock1, pBlock2, size)

OGE_API const char* ogeMemoryGetDebugInfo();
OGE_API const char* ogeMemoryTagToString(OgeMemoryTag memoryTag);

