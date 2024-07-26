#include <stdio.h>
#include <string.h>

#include "opl/opl.h"

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

char pMemoryDebugStr[MAX_DEBUG_INFO_LENGTH];

char* pMemoryTagNames[OGE_MEMORY_TAG_MAX_ENUM] = {
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

struct {
  b8 initialized;
  u64 totalUsage; 
  u64 perTagUsage[OGE_MEMORY_TAG_MAX_ENUM];
} s_state = { .initialized = OGE_FALSE };
#endif


void ogeMemoryInit() {
#ifdef OGE_DEBUG
  if (s_state.initialized) { 
    OGE_WARN("Trying to initialize memory system while it's already initialized.");
    return;
  }

  oplMemSet(&s_state, 0, sizeof(s_state));
  s_state.initialized = OGE_TRUE;

  OGE_INFO("Memory system initialized.");
#endif
}

void ogeMemoryTerminate() {
#ifdef OGE_DEBUG
  if (!s_state.initialized) {
    OGE_WARN("Trying to termiate memory system while it's already terminated.");
    return;
  }

  OGE_INFO("Memory system terminated.");
#endif
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

  OgeMemoryDebugHeader *pBlockHeader = oplAlloc(fullSize);
  pBlockHeader->size = size;
  pBlockHeader->tag  = memoryTag;

  s_state.totalUsage += size;
  s_state.perTagUsage[memoryTag] += size;
  
  return MEMORY_HTOS(pBlockHeader);
}

void* ogeRealloc(void *pBlock, u64 size) {
  OgeMemoryDebugHeader *pBlockHeader = MEMORY_STOH(pBlock);

  s_state.totalUsage -= pBlockHeader->size - size;
  s_state.perTagUsage[pBlockHeader->tag] -= pBlockHeader->size - size;

  pBlockHeader = oplRealloc(pBlockHeader,
                            sizeof(OgeMemoryDebugHeader) + size);
  pBlockHeader->size = size;

  return MEMORY_HTOS(pBlockHeader);
}

void ogeFree(void *pBlock) {
  const OgeMemoryDebugHeader *pBlockHeader = MEMORY_STOH(pBlock);

  s_state.totalUsage -= pBlockHeader->size;
  s_state.perTagUsage[pBlockHeader->tag] -= pBlockHeader->size;

  oplFree(MEMORY_STOH(pBlock));
}
#endif

const char* ogeMemoryGetDebugInfo() {
  #ifdef OGE_DEBUG
  const u32 gib = 1024 * 1024 * 1024;
  const u32 mib = 1024 * 1024;
  const u32 kib = 1024;

  ogeMemSet(pMemoryDebugStr, 0, sizeof(pMemoryDebugStr));
  snprintf(pMemoryDebugStr, MAX_DEBUG_INFO_LENGTH, "Memory usage:\n");
  u16 offset = strlen(pMemoryDebugStr);

  for (int i = 0; i < OGE_MEMORY_TAG_MAX_ENUM; ++i) {
    OGE_ASSERT(offset < MAX_DEBUG_INFO_LENGTH, "Memory debug info string is exceed the limit."); 

    char unit[4] = "Xib";
    f64 amount = s_state.perTagUsage[i];
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

    u16 length = snprintf(pMemoryDebugStr + offset, MAX_DEBUG_INFO_LENGTH,
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

const char* ogeMemoryTagToString(OgeMemoryTag memoryTag) {
  #ifdef OGE_DEBUG
  return pMemoryTagNames[memoryTag];
  #else
  return "";
  #endif
}
