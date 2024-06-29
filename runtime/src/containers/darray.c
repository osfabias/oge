#include "oge/core/logging.h"
#include "oge/defines.h"
#include "oge/core/memory.h"
#include "oge/containers/darray.h"

typedef struct OgeDArrayHeader {
  u32 capacity; // max 4'294'967'296
  u32 length;   // max 4'294'967'296
  u16 stride;   // max 8Kib (65'356 bits)
} OgeDArrayHeader;

#define DARRAY_SIZE(pDArrayHeader) \
  sizeof(OgeDArrayHeader) + (*pDArrayHeader).capacity * (*pDArrayHeader).stride

void* ogeDArrayAllocate(u32 length, u16 stride) {
  u64 arraySize = length * stride;
  u64 fullSize  = sizeof(OgeDArrayHeader) + arraySize;
  
  OgeDArrayHeader *pDArrayHeader = ogeAllocate(fullSize, OGE_MEMORY_TAG_DARRAY);
  ogeMemorySet(pDArrayHeader, 0, fullSize);

  (*pDArrayHeader).capacity = length;
  (*pDArrayHeader).length   = 0;
  (*pDArrayHeader).stride   = stride;

  return (void*)(pDArrayHeader + 1);
}

void ogeDArrayDeallocate(void *pDArray) {
  OgeDArrayHeader *pDArrayHeader = (OgeDArrayHeader*)pDArray - 1;
  ogeDeallocate(pDArrayHeader, DARRAY_SIZE(pDArrayHeader), OGE_MEMORY_TAG_DARRAY);
}

u32 ogeDArrayLength(void *pDArray) {
  return ((OgeDArrayHeader*)pDArray)->length;
}

u16 ogeDArrayStride(void *pDArray) {
  return ((OgeDArrayHeader*)pDArray)->length;
}

void* ogeDArrayResize(void *pDArray, u32 length) {
  #ifdef OGE_DEBUG
  if (length == 0) {
    OGE_WARN("ogeDArrayResize called with 0 length.");
  }
  #endif

  OgeDArrayHeader *pDArrayHeader = (OgeDArrayHeader*)pDArray - 1;
  u64 oldSize        = DARRAY_SIZE(pDArrayHeader);
  u64 newSize        = sizeof(OgeDArrayHeader) + length * (*pDArrayHeader).stride;

  OgeDArrayHeader *pNewDArrayHeader = ogeAllocate(newSize, OGE_MEMORY_TAG_DARRAY);

  if (length < (*pDArrayHeader).length) {
    ogeMemoryCopy(pNewDArrayHeader, pDArrayHeader, newSize);
    (*pNewDArrayHeader).length = length;
  }
  else {
    ogeMemorySet(pNewDArrayHeader, 0, newSize);
    ogeMemoryCopy(pNewDArrayHeader, pDArrayHeader, oldSize);
  }

  // Length & stride are copied automaticly
  (*pNewDArrayHeader).capacity = length;

  ogeDeallocate(pDArrayHeader, oldSize, OGE_MEMORY_TAG_DARRAY);

  return (void*)(pNewDArrayHeader + 1);
}

void* ogeDArrayShrink(void *pDArray) {
  OgeDArrayHeader *pDArrayHeader = (OgeDArrayHeader*)pDArray - 1;
  u64 oldSize        = DARRAY_SIZE(pDArrayHeader);
  u64 newSize        = sizeof(OgeDArrayHeader) + (*pDArrayHeader).length * (*pDArrayHeader).stride;

  OgeDArrayHeader *pNewDArrayHeader = ogeAllocate(newSize, OGE_MEMORY_TAG_DARRAY);
  ogeMemoryCopy(pNewDArrayHeader, pDArrayHeader, newSize);

  // Stride copied automaticly
  (*pNewDArrayHeader).capacity = (*pDArrayHeader).length;
  (*pNewDArrayHeader).length   = 0;

  ogeDeallocate(pDArrayHeader, oldSize, OGE_MEMORY_TAG_DARRAY);

  return (void*)(pNewDArrayHeader + 1);
}
