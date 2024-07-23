#include "oge/defines.h"
#include "oge/core/memory.h"
#include "oge/core/logging.h"
#include "oge/containers/darray.h"

typedef struct OgeDArrayHeader {
  u64 capacity;
  u64 length;
  u64 stride;
} OgeDArrayHeader;

#define DARRAY_RESIZE_FACTOR 1.5f

#define DARRAY_SIZE(length, stride) \
  sizeof(OgeDArrayHeader) + (length) * (stride)

// header to darray data start conversion
#define DARRAY_HTOS(ptr) \
  ((void*)((ptr) + 1))

// darray data start to header conversion
#define DARRAY_STOH(ptr) \
  (((OgeDArrayHeader*)(ptr)) - 1)

void* ogeDArrayAllocate(u64 length, u64 stride) {
  OgeDArrayHeader *pDArrayHeader =
    ogeAllocate(DARRAY_SIZE(length, stride), OGE_MEMORY_TAG_DARRAY);

  pDArrayHeader->capacity = length;
  pDArrayHeader->length   = 0;
  pDArrayHeader->stride   = stride;

  return DARRAY_HTOS(pDArrayHeader);
}

void ogeDArrayDeallocate(void *pDArray) {
  ogeDeallocate(DARRAY_STOH(pDArray));
}

void* ogeDArrayResize(void *pDArray, u64 length) {
  OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);

  #ifdef OGE_DEBUG
  if (length == 0) {
    OGE_WARN("DArray %p length is set to 0.", pDArrayHeader);
  }
  #endif

  const u64 newSize = DARRAY_SIZE(length, pDArrayHeader->stride);
  pDArrayHeader = ogeReallocate(pDArrayHeader, newSize);

  pDArrayHeader->capacity = length;
  pDArrayHeader->length = OGE_MIN(pDArrayHeader->length, length);

  return DARRAY_HTOS(pDArrayHeader);
}

void* ogeDArrayShrink(void *pDArray) {
  return ogeDArrayResize(pDArray, DARRAY_STOH(pDArray)->length);
}

void* ogeDArrayAppend(void *pDArray, const void *pValue) {
  OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);
  const u64 length = pDArrayHeader->length;
  const u64 stride = pDArrayHeader->stride;

  if (length == pDArrayHeader->capacity) {
    const u64 newSize = OGE_MAX(1, length) * DARRAY_RESIZE_FACTOR;
    pDArray = ogeDArrayResize(pDArray, newSize);
    pDArrayHeader = DARRAY_STOH(pDArray);
  }

  ogeMemoryCopy(((u8*)pDArray) + length * stride, pValue, stride);
  pDArrayHeader->length += 1;

  return pDArray;
}

void* ogeDArrayInsert(void *pDArray, u64 index, const void *pValue) {
  OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);
  const u64 length = pDArrayHeader->length;

  #if OGE_DEBUG
  if (index >= length) {
    OGE_ERROR("Inserting an element at the index %d that is outside the bounds of %p.", 
              index, pDArrayHeader);
    return pDArray;
  }
  #endif

  if (length == pDArrayHeader->capacity) {
    pDArray = ogeDArrayResize(pDArray, length * DARRAY_RESIZE_FACTOR);
    pDArrayHeader = DARRAY_STOH(pDArray);
  }

  ogeMemoryMove(((u8*)pDArray) + pDArrayHeader->stride * (index + 1),
                ((u8*)pDArray) + pDArrayHeader->stride * index,
                pDArrayHeader->length * pDArrayHeader->stride);
  ogeMemoryCopy(pDArray, pValue, pDArrayHeader->stride);
  pDArrayHeader->length += 1;

  return pDArray;
}

void* ogeDArrayExtend(void *pDArray, const void *pSrcBlock, u64 length) {
  OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);

  if (pDArrayHeader->length + length > pDArrayHeader->capacity) {
    const u64 l1 = length * DARRAY_RESIZE_FACTOR;
    const u64 l2 = pDArrayHeader->length + length;
    const u64 newLength = OGE_MAX(l1, l2);
    pDArray = ogeDArrayResize(pDArray, newLength);
    pDArrayHeader = DARRAY_STOH(pDArray);
  }

  ogeMemoryCopy(
    ((u8*)pDArray) + pDArrayHeader->length * pDArrayHeader->stride,
    pSrcBlock, pDArrayHeader->stride * length);
  pDArrayHeader->length += length;
  return pDArray;
}

void ogeDArrayPop(void *pDArray, void *pOut) {
  OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);
  pDArrayHeader->length -= 1;
  ogeMemoryCopy(pOut, ((u8*)pDArray + pDArrayHeader->length * pDArrayHeader->stride),
                pDArrayHeader->stride);
}

void ogeDArrayPopAt(void *pDArray, u64 index, void *pOut) {
  OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);

  #if OGE_DEBUG
  if (index >= pDArrayHeader->length) {
    OGE_ERROR("Popping an element at the index %d that is outside the bounds of %p.", index, pDArray);
    return;
  }
  #endif

  ogeMemoryCopy(pOut, ((u8*)pDArray) + pDArrayHeader->stride * index,
                pDArrayHeader->stride);
  ogeMemoryMove(((u8*)pDArray) + pDArrayHeader->stride * index,
                ((u8*)pDArray) + pDArrayHeader->stride * (index + 1),
                pDArrayHeader->length * pDArrayHeader->stride);
  pDArrayHeader->length -= 1;
}

void ogeDArrayRemove(void *pDArray, u64 index) {
  OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);

  #if OGE_DEBUG
  if (index >= pDArrayHeader->length) {
    OGE_ERROR("Removing an element at the index %d that is outside the bounds of %p.", index, pDArray);
    return;
  }
  #endif

  ogeMemoryMove(((u8*)pDArray) + pDArrayHeader->stride * index,
                ((u8*)pDArray) + pDArrayHeader->stride * (index + 1),
                pDArrayHeader->length * pDArrayHeader->stride);
  pDArrayHeader->length -= 1;
}

u64 ogeDArrayFind(void *pDArray, const void *pValue) {
  const OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);
  const u64 stride = pDArrayHeader->stride;

  for (u64 i = 0; i < pDArrayHeader->length; ++i) {
    if (ogeMemoryCompare((u8*)pDArray + stride * i, pValue ,stride) == 0) {
      return i;
    }
  }

  return -1;
}
