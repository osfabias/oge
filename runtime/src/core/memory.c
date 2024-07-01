#include <stdio.h>
#include <string.h>

#include "oge/core/memory.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/assertion.h"

#ifdef OGE_DEBUG

#define _MAX_DEBUG_INFO_LENGTH 8192

#define MEMORY_HTOS(ptr) \
  ((void*)((ptr) + 1))

#define MEMORY_STOH(ptr) \
  (((OgeMemoryDebugHeader*)(ptr)) - 1)

typedef struct OgeMemoryDebugHeader {
  u64 size;
  u16 tag;
} OgeMemoryDebugHeader;

struct {
  u64 totalUsage; 
  u64 perTagUsage[OGE_MEMORY_TAG_MAX_ENUM];
} ogeMemoryDebugStats;

char* pMemoryTagNames[OGE_MEMORY_TAG_MAX_ENUM] = {
  "UNKNOWN    ",
  "ARRAY      ",
  "DARRAY     ",
  "DICT       ",
  "RING_QUEUE ",
  "BST        ",
  "STRING     ",
  "APPLICATION",
  "JOB        ",
  "TEXTURE    ",
  "MATERIAL   ",
  "RENDERER   ",
  "GAME       ",
  "TRANSFORM  ",
  "ENTITY     ",
  "ENTITY NODE",
  "SCENE      ",
};

char pMemoryDebugStr[_MAX_DEBUG_INFO_LENGTH];

#endif

void ogeMemoryInit() {
  #ifdef OGE_DEBUG
    ogePlatformMemorySet(&ogeMemoryDebugStats, 0, sizeof(ogeMemoryDebugStats));
  #endif
}

void ogeMemoryTerminate() { }

void* ogeAllocate(u64 size, OgeMemoryTag memoryTag) {
  #ifdef OGE_DEBUG
  if (memoryTag == OGE_MEMORY_TAG_UNKNOWN) {
    OGE_WARN("ogeAllocate called with OGE_MEMORY_TAG_UNKNOWN. Set memory tag to different.");
  }

  if (size == 0) {
    OGE_WARN("ogeAllocate called with 0 size.");
  }
  u64 fullSize = sizeof(OgeMemoryDebugHeader) + size;

  OgeMemoryDebugHeader *pBlockHeader = ogePlatformAllocate(fullSize, OGE_FALSE);
  pBlockHeader->size = size;
  pBlockHeader->tag  = memoryTag;

  ogeMemoryDebugStats.totalUsage += size;
  ogeMemoryDebugStats.perTagUsage[memoryTag] += size;

  return MEMORY_HTOS(pBlockHeader);
  #else
  return ogePlatformAllocate(size, OGE_FALSE);
  #endif
}

void* ogeReallocate(void *pBlock, u64 size) {
  #ifdef OGE_DEBUG
  OgeMemoryDebugHeader *pBlockHeader = MEMORY_STOH(pBlock);

  ogeMemoryDebugStats.totalUsage -= pBlockHeader->size - size;
  ogeMemoryDebugStats.perTagUsage[pBlockHeader->tag] -= pBlockHeader->size - size;

  pBlockHeader = ogePlatformReallocate(pBlockHeader,
                                       sizeof(OgeMemoryDebugHeader) + size, OGE_FALSE);
  pBlockHeader->size = size;

  return MEMORY_HTOS(pBlockHeader);
  #else
  return ogePlatformReallocate(pBlock, size, OGE_FALSE);
  #endif
}

void ogeDeallocate(void *pBlock) {
  #ifdef OGE_DEBUG
  OgeMemoryDebugHeader *pBlockHeader = MEMORY_STOH(pBlock);

  ogeMemoryDebugStats.totalUsage -= pBlockHeader->size;
  ogeMemoryDebugStats.perTagUsage[pBlockHeader->tag] -= pBlockHeader->size;

  ogePlatformDeallocate(pBlockHeader, OGE_FALSE);
  #else
  ogePlatformDeallocate(pBlock, OGE_FALSE);
  #endif
}

void ogeMemoryCopy(void *pDstBlock, const void *pSrcBlock, u64 size) {
  ogePlatformMemoryCopy(pDstBlock, pSrcBlock, size);
}

void ogeMemorySet(void *block, i32 value, u64 size) {
  ogePlatformMemorySet(block, value, size);
}

void ogeMemoryMove(void *pDstBlock, const void *pSrcBlock, u64 size) {
  ogePlatformMemoryMove(pDstBlock, pSrcBlock, size);
}

const char* ogeMemoryGetDebugInfo() {
  #ifdef OGE_DEBUG
  const u32 gib = 1024 * 1024 * 1024;
  const u32 mib = 1024 * 1024;
  const u32 kib = 1024;

  ogeMemorySet(pMemoryDebugStr, 0, sizeof(pMemoryDebugStr));
  snprintf(pMemoryDebugStr, _MAX_DEBUG_INFO_LENGTH, "Memory usage:\n");
  u16 offset = strlen(pMemoryDebugStr);

  for (int i = 0; i < OGE_MEMORY_TAG_MAX_ENUM; ++i) {
    OGE_ASSERT(offset < _MAX_DEBUG_INFO_LENGTH, "Memory debug info string is exceed the limit."); 

    char unit[4] = "Xib";
    f64 amount = ogeMemoryDebugStats.perTagUsage[i];
    if (amount >= gib) {
      unit[0] = 'G';
      amount /= gib;
    }
    else if (amount >= mib) {
      unit[0] = 'M';
      amount /= mib;
    }
    else if (amount >= kib) {
      unit[0] = 'K';
      amount /= kib;
    }
    else {
      unit[0] = 'B';
      unit[1] = '\0';
    }

    u16 length = snprintf(pMemoryDebugStr + offset, _MAX_DEBUG_INFO_LENGTH,
                          "%s : %.2f %s\n", pMemoryTagNames[i], amount, unit);
    offset += length;
  }

  // Remove last '\n' symbol
  pMemoryDebugStr[offset - 1] = '\0';
  return pMemoryDebugStr;
  #else
  return "Memory debug info unavailable in release build.";
  #endif
}
