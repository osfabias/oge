#pragma once

#include "oge/defines.h"

/**
 * @brief Allocates a dynamic array.
 * @param length Size of a dynamic array in elements.
 * @param stride Size of the each individual element.
 */
OGE_API void* ogeDArrayAllocate(u32 length, u16 stride);

/**
 * @brief Deallocates a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 */
OGE_API void ogeDArrayDeallocate(void *pDArray);

/**
 * @brief Returns a length (amount of elements) of a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 */
OGE_API u32 ogeDArrayLength(void *pDArray);

/**
 * @brief Return a stride (size of an element) of a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 */
OGE_API u16 ogeDArrayStride(void *pDArray);

/**
 * @brief Resizes a dynamic array to the specified length.
 * @param pDArray A pointer to a dynamic array.
 * @param length A length to resize to.
 */
OGE_API void* ogeDArrayResize(void *pDArray, u32 length);

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
OGE_API void ogeDArrayPush(void *pDArray, const void *pValue);

/**
 * @brief Pushes a given value to the start (left side) of a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 * @param pValue A pointer to a value to push.
 */
OGE_API void ogeDArrayPushBack(void *pDArray, const void *pValue);

/**
 * @brief Inserts a given value at specified index in a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 * @param index An index to insert at.
 * @param pValue A pointer to a value to push.
 */
OGE_API void ogeDArrayInsert(void *pDArray, u32 index, const void *pValue);

/**
 * @brief Pops a value from the end (right side) of a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 */
OGE_API void* ogeDArrayPop(void *pDArray);

/**
 * @brief Pops a value from the start (left side) of a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 */
OGE_API void* ogeDArrayPopBack(void *pDArray);

/**
 * @brief Pops a value at the given index in a dynamic array.
 * @param pDArray A pointer to a dynamic array.
 * @param index An index to pop from.
 */
OGE_API void* ogeDArrayPopAt(void *pDArray, u32 index);
