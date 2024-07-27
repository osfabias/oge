/**
 * @file darray.h
 * @brief The header of the darray
 *
 * Copyright (c) 2023-2024 Osfabias
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "oge/defines.h"

/**
 * @brief Allocates a darray.
 * @param length A size of a darray in elements.
 * @param stride A size of the each individual element in bytes.
 * @returns Returns a pointer to the allocated darray.
 */
OGE_API void* ogeDArrayAlloc(u64 length, u64 stride);

/**
 * @brief Frees a darray.
 * @param darray A pointer to a darray.
 */
OGE_API void ogeDArrayFree(void *darray);

/**
 * @brief Returns a length (amount of elements) of a darray.
 * @param darray A pointer to a darray.
 * @returns Returns a length of the given darray.
 */
#define ogeDArrayLength(darray) \
  *((u64*)darray - 2) // see darray.c for darray header structure

/**
 * @brief Returns a stride (size of an element) of a darray.
 * @param darray A pointer to a darray.
 */
#define ogeDArrayStride(darray) \
  *((u64*)darray - 1) // see darray.c for darray header structure

/**
 * @brief Resizes the given darray to the specified length.
 * @param darray A pointer to a darray.
 * @param length A length in elements to resize to.
 * @returns Returns a pointer to the new resized darray.
 */
OGE_API void* ogeDArrayResize(void *darray, u64 length);

/**
 * @brief Shrinks a darray's capacity to it's length.
 * @param darray A pointer to a darray.
 * @return Returns a pointer to the new resized darray.
 */
OGE_API OGE_INLINE void* ogeDArrayShrink(void *darray) {
  return ogeDArrayResize(darray, ogeDArrayLength(darray));
}

/**
 * @brief Copies a value to the end of a darray.
 * @param darray A pointer to a darray.
 * @param value A pointer to a value to copy.
 * @return Returns a pointer to the old darray or if there wasn't 
 *         enough space for inserting a value to the new reallocated
 *         darray.
 * new reallocated darray.
 */
OGE_API void* ogeDArrayAppend(void *darray, const void *value);

/**
 * @brief Extends a dynamic array and copies a memory block
 *        to the end of a darray.
 * @param darray A pointer to a darray.
 * @param src A pointer to a source memory block to copy from.
 * @param length A length of the source memory block in elements.
 *               Stride will be determined from the passed darray.
 * @return Returns a pointer to the old darray or if there wasn't 
 *         enough space for inserting a value to the new reallocated
 *         darray.
 */
OGE_API void* ogeDArrayExtend(void *darray, const void *src, u64 length);

/**
 * @brief Copies a value to the specified index in a darray.
 * @param darray A pointer to a darray.
 * @param index An index to copy to.
 * @param value A pointer to a value to copy.
 * @return Returns a pointer to the old darray or if there wasn't 
 *         enough space for inserting a value to the new reallocated
 *         darray.
 */
OGE_API void* ogeDArrayInsert(void *dArray, u64 index, const void *value);

/**
 * @brief Copies a value from the end of a darray and removes it
 *        from darray.
 * @param darray A pointer to a darray.
 * @param out A pointer to a variable to copy to.
 */
OGE_API void ogeDArrayPop(void *darray, void *out);

/**
 * @brief Copies a value from the given index in a darray.
 * @param darray A pointer to a darray.
 * @param index An index to pop from.
 * @param out A pointer to a variable to copy to.
 */
OGE_API void ogeDArrayPopAt(void *darray, u64 index, void *out);

/**
 * @brief Removes an element at the given index from a darray.
 * @param darray A pointer to a darray.
 * @param index An index to remove from.
 */
OGE_API void ogeDArrayRemove(void *darray, u64 index);

/**
 * @brief Clears darray.
 *
 * Technicaly, it just means, that the length of a darray
 * is set to 0.
 *
 * @param darray A pointer to a darray.
 */
#define ogeDArrayClear(darray) \
  *((u64*)pDArray - 2) = 0 // see darray.c for darray header structure

/**
 * @brief Returns an index of the first appearance of a value.
 * @param darray A pointer to a darray.
 * @param value A pointer to a value to find.
 * @return Returns an index of the first appearance of a value
 *         or if it wasn't found returns -1.
 */
OGE_API u64 ogeDArrayFind(void *darray, const void *value);
