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

/* a darray header to a darray start conversion */
#define DARRAY_HTOS(darray) \
  ((void*)((darray) + 1))

/* a darray start to a darray header conversion */
#define DARRAY_STOH(darray) \
  (((OgeDArrayHeader*)(darray)) - 1)

void* ogeDArrayAlloc(u64 length, u64 stride) {
  OgeDArrayHeader *darrayHeader =
    ogeAlloc(DARRAY_SIZE(length, stride), OGE_MEMORY_TAG_DARRAY);

  darrayHeader->capacity = length;
  darrayHeader->length   = 0;
  darrayHeader->stride   = stride;

  return DARRAY_HTOS(darrayHeader);
}

void ogeDArrayFree(void *darray) {
  ogeFree(DARRAY_STOH(darray));
}

void* ogeDArrayResize(void *darray, u64 length) {
  OgeDArrayHeader *darrayHeader = DARRAY_STOH(darray);

  #ifdef OGE_DEBUG
  if (length == 0) {
    OGE_WARN("DArray %p length is set to 0.", darrayHeader);
  }
  #endif

  const u64 newSize = DARRAY_SIZE(length, darrayHeader->stride);
  darrayHeader = ogeRealloc(darrayHeader, newSize);

  darrayHeader->capacity = length;
  darrayHeader->length = OGE_MIN(darrayHeader->length, length);

  return DARRAY_HTOS(darrayHeader);
}

void* ogeDArrayAppend(void *darray, const void *value) {
  OgeDArrayHeader *darrayHeader = DARRAY_STOH(darray);
  const u64 length = darrayHeader->length;
  const u64 stride = darrayHeader->stride;

  if (length == darrayHeader->capacity) {
    const u64 newSize = OGE_MAX(1, length) * DARRAY_RESIZE_FACTOR;
    darray = ogeDArrayResize(darray, newSize);
    darrayHeader = DARRAY_STOH(darray);
  }

  ogeMemCpy(((u8*)darray) + length * stride, value, stride);
  darrayHeader->length += 1;

  return darray;
}

void* ogeDArrayInsert(void *darray, u64 index, const void *value) {
  OgeDArrayHeader *darrayHeader = DARRAY_STOH(darray);
  const u64 length = darrayHeader->length;

  #if OGE_DEBUG
  if (index >= length) {
    OGE_ERROR("Inserting an element at the index %d that is outside the bounds of %p.", 
              index, darrayHeader);
    return darray;
  }
  #endif

  if (length == darrayHeader->capacity) {
    darray = ogeDArrayResize(darray, length * DARRAY_RESIZE_FACTOR);
    darrayHeader = DARRAY_STOH(darray);
  }

  ogeMemMove(((u8*)darray) + darrayHeader->stride * (index + 1),
                ((u8*)darray) + darrayHeader->stride * index,
                darrayHeader->length * darrayHeader->stride);
  ogeMemCpy(darray, value, darrayHeader->stride);
  darrayHeader->length += 1;

  return darray;
}

void* ogeDArrayExtend(void *darray, const void *pSrcBlock, u64 length) {
  OgeDArrayHeader *darrayHeader = DARRAY_STOH(darray);

  if (darrayHeader->length + length > darrayHeader->capacity) {
    const u64 l1 = length * DARRAY_RESIZE_FACTOR;
    const u64 l2 = darrayHeader->length + length;
    const u64 newLength = OGE_MAX(l1, l2);
    darray = ogeDArrayResize(darray, newLength);
    darrayHeader = DARRAY_STOH(darray);
  }

  ogeMemCpy(
    ((u8*)darray) + darrayHeader->length * darrayHeader->stride,
    pSrcBlock, darrayHeader->stride * length);
  darrayHeader->length += length;
  return darray;
}

void ogeDArrayPop(void *darray, void *pOut) {
  OgeDArrayHeader *darrayHeader = DARRAY_STOH(darray);
  darrayHeader->length -= 1;
  ogeMemCpy(pOut, ((u8*)darray + darrayHeader->length * darrayHeader->stride),
                darrayHeader->stride);
}

void ogeDArrayPopAt(void *darray, u64 index, void *pOut) {
  OgeDArrayHeader *darrayHeader = DARRAY_STOH(darray);

  #if OGE_DEBUG
  if (index >= darrayHeader->length) {
    OGE_ERROR("Popping an element at the index %d that is outside the bounds of %p.", index, darray);
    return;
  }
  #endif

  ogeMemCpy(pOut, ((u8*)darray) + darrayHeader->stride * index,
                darrayHeader->stride);
  ogeMemMove(((u8*)darray) + darrayHeader->stride * index,
                ((u8*)darray) + darrayHeader->stride * (index + 1),
                darrayHeader->length * darrayHeader->stride);
  darrayHeader->length -= 1;
}

void ogeDArrayRemove(void *darray, u64 index) {
  OgeDArrayHeader *darrayHeader = DARRAY_STOH(darray);

  #if OGE_DEBUG
  if (index >= darrayHeader->length) {
    OGE_ERROR("Removing an element at the index %d that is outside the bounds of %p.", index, darray);
    return;
  }
  #endif

  ogeMemMove(((u8*)darray) + darrayHeader->stride * index,
                ((u8*)darray) + darrayHeader->stride * (index + 1),
                darrayHeader->length * darrayHeader->stride);
  darrayHeader->length -= 1;
}

u64 ogeDArrayFind(void *darray, const void *value) {
  const OgeDArrayHeader *darrayHeader = DARRAY_STOH(darray);
  const u64 stride = darrayHeader->stride;

  for (u64 i = 0; i < darrayHeader->length; ++i) {
    if (ogeMemCmp((u8*)darray + stride * i, value ,stride) == 0) {
      return i;
    }
  }

  return -1;
}
