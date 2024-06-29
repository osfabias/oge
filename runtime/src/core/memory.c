#include <stdio.h>
#include <string.h>

#define _MAX_DEBUG_INFO_LENGTH 8192

#include "oge/core/memory.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/assertion.h"

struct {
  u64 totalUsage; 
  u64 perTagUsage[OGE_MEMORY_TAG_MAX_ENUM];
} ogeMemoryStats;

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

char pDebugInfo[_MAX_DEBUG_INFO_LENGTH];

void ogeMemoryInit() {
  ogePlatformMemorySet(&ogeMemoryStats, 0, sizeof(ogeMemoryStats));
}

void ogeMemoryTerminate() {
  ogePlatformMemorySet(&ogeMemoryStats, 0, sizeof(ogeMemoryStats));
}

void* ogeAllocate(u64 size, OgeMemoryTag memoryTag) {
  #ifdef OGE_DEBUG
  if (memoryTag == OGE_MEMORY_TAG_UNKNOWN) {
    OGE_WARN("ogeAllocate called with OGE_MEMORY_TAG_UNKNOWN. Set memory tag to different.");
  }

  if (size == 0) {
    OGE_WARN("ogeAllocate called with 0 size.");
  }
  #endif

  ogeMemoryStats.totalUsage += size;
  ogeMemoryStats.perTagUsage[memoryTag] += size;

  return ogePlatformAllocate(size, OGE_FALSE);
}

void ogeDeallocate(void *block, u64 size, OgeMemoryTag memoryTag) {
  #ifdef OGE_DEBUG
  if (memoryTag == OGE_MEMORY_TAG_UNKNOWN) {
    OGE_WARN("ogeDeallocate called with OGE_MEMORY_TAG_UNKNOWN. Set memory tag to different.");
  }
  #endif

  ogeMemoryStats.totalUsage -= size;
  ogeMemoryStats.perTagUsage[memoryTag] -= size;

  ogePlatformDeallocate(block, OGE_FALSE);
}

void ogeMemoryCopy(void *dst, const void *src, u64 size) {
  ogePlatformMemoryCopy(dst, src, size);
}

void ogeMemorySet(void *block, i32 value, u64 size) {
  ogePlatformMemorySet(block, value, size);
}

const char* ogeMemoryGetDebugInfo() {
  const u32 gib = 1024 * 1024 * 1024;
  const u32 mib = 1024 * 1024;
  const u32 kib = 1024;

  ogeMemorySet(pDebugInfo, 0, sizeof(pDebugInfo));
  snprintf(pDebugInfo, _MAX_DEBUG_INFO_LENGTH, "Memory usage:\n");
  u16 offset = strlen(pDebugInfo);

  for (int i = 0; i < OGE_MEMORY_TAG_MAX_ENUM; ++i) {
    OGE_ASSERT(offset < _MAX_DEBUG_INFO_LENGTH, "Memory debug info string is exceed the limit."); 

    char unit[4] = "Xib";
    f64 amount = ogeMemoryStats.perTagUsage[i];
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

    u16 length = snprintf(pDebugInfo + offset, _MAX_DEBUG_INFO_LENGTH,
                          "%s : %.2f %s\n", pMemoryTagNames[i], amount, unit);
    offset += length;
  }

  // Remove last '\n' symbol
  pDebugInfo[offset - 1] = '\0';
  return pDebugInfo;
}
