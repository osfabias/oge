#include <stdio.h>
#include <string.h>

#include <opl/opl.h>

#include "oge/core/memory.h"
#include "oge/core/logging.h"
#include "oge/core/assertion.h"

#ifdef OGE_DEBUG
#define MAX_DEBUG_INFO_LENGTH 8192

#define MEMORY_HTOS(ptr) \
  ((void*)((ptr) + 1))

#define MEMORY_STOH(ptr) \
  (((OgeMemoryDebugHeader*)(ptr)) - 1)

typedef struct OgeMemoryDebugHeader {
  u64 size;
  u16 tag;
} OgeMemoryDebugHeader;

char s_memoryDebugInfo[MAX_DEBUG_INFO_LENGTH];

char* s_memoryTagNames[OGE_MEMORY_TAG_MAX_ENUM] = {
  "UNKNOWN",
  "ARRAY",
  "DARRAY",
  "DICT",
  "STRING",
  "APPLICATION",
  "TEXTURE",
  "MATERIAL",
  "RENDERER",
  "GAME",
  "TRANSFORM",
  "ENTITY",
  "ENTITY NODE",
  "SCENE",
};
#endif

static struct {
  b8 initialized;
#ifdef OGE_DEBUG
  u64 totalUsage; 
  u64 perTagUsage[OGE_MEMORY_TAG_MAX_ENUM];
#endif
} s_memoryState = { .initialized = OGE_FALSE };

void ogeMemoryInit() {
  OGE_ASSERT(
    !s_memoryState.initialized,
    "Trying to initialize memory system while it's already initialized."
  );

#ifdef OGE_DEBUG
  oplMemSet(&s_memoryState, 0, sizeof(s_memoryState));
  s_memoryState.initialized = OGE_TRUE;
#endif

  OGE_INFO("Memory system initialized.");
}

void ogeMemoryTerminate() {
  OGE_ASSERT(
    s_memoryState.initialized,
    "Trying to initialize memory system while it's already initialized."
  );

  OGE_INFO("Memory system terminated.");
}

#ifdef OGE_DEBUG
void* ogeAlloc(u64 size, OgeMemoryTag memoryTag) {
  if (memoryTag == OGE_MEMORY_TAG_UNKNOWN) {
    OGE_WARN("ogeAllocate called with OGE_MEMORY_TAG_UNKNOWN. Set memory tag to different.");
  }

  if (size == 0) {
    OGE_WARN("ogeAllocate called with 0 size.");
  }
  u64 fullSize = sizeof(OgeMemoryDebugHeader) + size;

  OgeMemoryDebugHeader *blockHeader = oplAlloc(fullSize);
  blockHeader->size = size;
  blockHeader->tag  = memoryTag;

  s_memoryState.totalUsage += size;
  s_memoryState.perTagUsage[memoryTag] += size;
  
  return MEMORY_HTOS(blockHeader);
}

void* ogeRealloc(void *block, u64 size) {
  OgeMemoryDebugHeader *blockHeader = MEMORY_STOH(block);

  s_memoryState.totalUsage -= blockHeader->size - size;
  s_memoryState.perTagUsage[blockHeader->tag] -= blockHeader->size - size;

  blockHeader = oplRealloc(blockHeader,
                            sizeof(OgeMemoryDebugHeader) + size);
  blockHeader->size = size;

  return MEMORY_HTOS(blockHeader);
}

void ogeFree(void *block) {
  const OgeMemoryDebugHeader *blockHeader = MEMORY_STOH(block);

  s_memoryState.totalUsage -= blockHeader->size;
  s_memoryState.perTagUsage[blockHeader->tag] -= blockHeader->size;

  oplFree(MEMORY_STOH(block));
}
#endif

const char* ogeMemoryGetDebugInfo() {
  #ifdef OGE_DEBUG
  const u32 gib = 1024 * 1024 * 1024;
  const u32 mib = 1024 * 1024;
  const u32 kib = 1024;

  ogeMemSet(s_memoryDebugInfo, 0, sizeof(s_memoryDebugInfo));
  snprintf(s_memoryDebugInfo, MAX_DEBUG_INFO_LENGTH, "Memory usage:\n");
  u16 offset = strlen(s_memoryDebugInfo);

  for (int i = 0; i < OGE_MEMORY_TAG_MAX_ENUM; ++i) {
    OGE_ASSERT(offset < MAX_DEBUG_INFO_LENGTH,
               "Memory debug info string is exceed the limit."); 

    char unit[4] = "Xib";
    f64 amount = s_memoryState.perTagUsage[i];
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

    u16 length = snprintf(s_memoryDebugInfo + offset, MAX_DEBUG_INFO_LENGTH,
                          "%s : %.2f %s\n", s_memoryTagNames[i], amount, unit);
    offset += length;
  }

  // Remove last '\n' symbol
  s_memoryDebugInfo[offset - 1] = '\0';
  return s_memoryDebugInfo;
  #else
  return ""; // may be it can cause some little memory leaks - check this
  #endif
}

const char* ogeMemoryTagToString(OgeMemoryTag memoryTag) {
  #ifdef OGE_DEBUG
  return s_memoryTagNames[memoryTag];
  #else
  return "";
  #endif
}
