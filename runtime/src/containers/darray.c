#include "oge/defines.h"
#include "oge/core/memory.h"
#include "oge/core/logging.h"
#include "oge/containers/darray.h"

typedef struct OgeDArrayHeader {
  u64 capacity;
  u64 length;
  u64 stride;
} OgeDArrayHeader;

#define DARRAY_RESIZE_FACTOR 1.25f

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

  OGE_TRACE("DArray allocated (%p):\ncapacity:%lu\nstride:%lu",
            pDArrayHeader, length, stride);

  return DARRAY_HTOS(pDArrayHeader);
}

void ogeDArrayDeallocate(void *pDArray) {
  OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);
  ogeDeallocate(pDArrayHeader);

  OGE_TRACE("DArray deallocated (%p)", pDArrayHeader);
}

void* ogeDArrayResize(void *pDArray, u64 length) {
  OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);

  pDArrayHeader = ogeReallocate(pDArrayHeader,
                                DARRAY_SIZE(length, pDArrayHeader->stride));

  pDArrayHeader->capacity = length;
  pDArrayHeader->length = OGE_MIN(pDArrayHeader->length, length);

  OGE_TRACE("DArray resized (%p -> %p): %lu", DARRAY_STOH(pDArray), pDArrayHeader, length);

  return DARRAY_HTOS(pDArrayHeader);
}

void* ogeDArrayShrink(void *pDArray) {
  return ogeDArrayResize(pDArray, OGE_MAX(DARRAY_STOH(pDArray)->length, 1));
}

void* ogeDArrayAppend(void *pDArray, const void *pValue) {
  OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);

  if (pDArrayHeader->length == pDArrayHeader->capacity) {
    pDArray = ogeDArrayResize(pDArray, pDArrayHeader->length * DARRAY_RESIZE_FACTOR);
    pDArrayHeader = DARRAY_STOH(pDArray);
  }

  ogeMemoryCopy(((u8*)pDArray) + pDArrayHeader->length * pDArrayHeader->stride,
                pValue, pDArrayHeader->stride);
  pDArrayHeader->length += 1;

  return pDArray;
}

void* ogeDArrayInsert(void *pDArray, u64 index, const void *pValue) {
  OgeDArrayHeader *pDArrayHeader = DARRAY_STOH(pDArray);

  if (pDArrayHeader->length == pDArrayHeader->capacity) {
    pDArray = ogeDArrayResize(pDArray, pDArrayHeader->length * DARRAY_RESIZE_FACTOR);
    pDArrayHeader = DARRAY_STOH(pDArray);
  }

  ogeMemoryMove(((u8*)pDArray) + pDArrayHeader->stride * (index + 1),
                ((u8*)pDArray) + pDArrayHeader->stride * index,
                pDArrayHeader->length * pDArrayHeader->stride);
  ogeMemoryCopy(pDArray, pValue, pDArrayHeader->stride);
  pDArrayHeader->length += 1;

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

  ogeMemoryCopy(pOut, ((u8*)pDArray) + pDArrayHeader->stride * index,
                pDArrayHeader->stride);
  ogeMemoryMove(((u8*)pDArray) + pDArrayHeader->stride * index,
                ((u8*)pDArray) + pDArrayHeader->stride * (index + 1),
                pDArrayHeader->length * pDArrayHeader->stride);
  pDArrayHeader->length -= 1;
}
