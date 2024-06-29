#pragma once
/************************************************
 *                   macros                     *
 ************************************************/
// ~ DLL export
#ifdef OGE_EXPORT
  #ifdef _MSC_VER
    #define OGE_API __declspec(dllexport)
  #else
    #define OGE_API __attribute__((visibility("default")))
  #endif
#else
  #ifdef _MSC_VER
    #define OGE_API __declspec(dllimport)
  #else
    #define OGE_API
  #endif
#endif

// ~ Static assertion
#ifdef _MSC_VER
  #define OGE_STATIC_ASSERT static_assert
#else // clang or gcc
  #define _OGE_STATIC_ASSERT _Static_assert
#endif

// ~ Inlining
#if defined(_MSC_VER)
  #define OGE_INLINE   __forceinline
  #define OGE_NOINLINE __declspec(noinline)
#else // clang or gcc
  #define OGE_INLINE   __attribute__((always_inline)) inline
  #define OGE_NOINLINE __attribute__((noinline))
#endif

// ~ Utils
#define CLAMP(x, l, r) (((x) <= (l)) ? (l) : ((x) >= (r)) ? (r) : (x))

#define OGE_MIN(x, y) ((x) < (y) ? (x) : (y))
#define OGE_MAX(x, y) ((x) > (y) ? (x) : (y))

#define OGE_GIBIBYTES(x) ((x) * 1024ULL * 1024ULL * 1024ULL)
#define OGE_MEBIBYTES(x) ((x) * 1024ULL * 1024ULL)
#define OGE_KIBIBYTES(x) ((x) * 1024ULL)

#define OGE_GIGABYTES(x) ((x) * 1000ULL * 1000ULL * 1000ULL)
#define OGE_MEGABYTES(x) ((x) * 1000ULL * 1000ULL)
#define OGE_KILOBYTES(x) ((x) * 1000ULL)

/************************************************
 *                 OGE tokens                   *
 ************************************************/
/** @brief True. */
#define OGE_TRUE 1

/** @brief False. */
#define OGE_FALSE 0

// Any id set to this should be considered invalid,
// and not actually pointing to a real object.
#define OGE_INVALID_ID_U64 18446744073709551615UL
#define OGE_INVALID_ID_U32 4294967295U
#define OGE_INVALID_ID_U16 65535U
#define OGE_INVALID_ID_U8  255U

/************************************************
 *                   types                      *
 ************************************************/
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

typedef _Bool b8;

// Check sizes to be exact as expected
_OGE_STATIC_ASSERT(sizeof(u8) == 1,  "Expected u8 to be 1 byte.");
_OGE_STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");
_OGE_STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");
_OGE_STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");

_OGE_STATIC_ASSERT(sizeof(i8) == 1,  "Expected i8 to be 1 byte.");
_OGE_STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes.");
_OGE_STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes.");
_OGE_STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes.");

_OGE_STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");
_OGE_STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");

