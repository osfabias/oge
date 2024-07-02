#pragma once

#include "oge/defines.h"

/**
 * @brief Allocates a dynamic array.
 * @param length Size of a dynamic array in elements.
 * @param stride Size of the each individual element.
 */
OGE_API void* ogeDArrayAllocate(u64 length, u64 stride);

/**
 * @brief Deallocates a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 */
OGE_API void ogeDArrayDeallocate(void *pDArray);

/**
 * @brief Returns a length (amount of elements) of a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 */
OGE_INLINE u64 ogeDArrayLength(void *pDArray) {
  return *((u64*)pDArray - 2); // see darray.c for header structure
}

/**
 * @brief Return a stride (size of an element) of a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 */
OGE_INLINE u64 ogeDArrayStride(void *pDArray) {
  return *((u64*)pDArray - 1); // see darray.c for header structure
}

/**
 * @brief Resizes a dynamic array to the specified length.
 * @param pDArray A pointer to a dynamic array.
 * @param length A length to resize to.
 */
OGE_API void* ogeDArrayResize(void *pDArray, u64 length);

/**
 * @brief Shrinks a dynamic array to it's current length.
 * @param pDArray A pointer to a dynamic array.
 */
OGE_API void* ogeDArrayShrink(void *pDArray);

/**
 * @brief Pushes a given value to the end (right side) of a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 * @param pValue A pointer to a value to push.
 */
OGE_API void* ogeDArrayAppend(void *pDArray, const void *pValue);

/**
 * @brief Inserts a given value at specified index in a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 * @param index An index to insert at.
 * @param pValue A pointer to a value to push.
 */
OGE_API void* ogeDArrayInsert(void *pDArray, u64 index, const void *pValue);

/**
 * @brief Pops a value from the end (right side) of a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 */
OGE_API void ogeDArrayPop(void *pDArray, void *pOut);

/**
 * @brief Pops a value at the given index in a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 * @param index An index to pop from.
 */
OGE_API void ogeDArrayPopAt(void *pDArray, u64 index, void *pOut);

/**
 * @brief Deletes an element at given index from a dynamic array.
 */
OGE_API void ogeDArrayRemove(void *pDArray, u64 index);

/**
 * @brief Returns an index of first appearance of pValue.
 * @param pDArray A pointer to a dynamic array.
 * @param pValue A pointer to a value to find.
 */
OGE_API u64 ogeDArrayFind(void *pDArray, const void *pValue);
