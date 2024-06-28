#pragma once
/************************************************
 *                   macros                     *
 ************************************************/
// ~ Static assertion
#ifdef _MSC_VER
  #define _OGE_STATIC_ASSERT static_assert
#else // clang or gcc
  #define _OGE_STATIC_ASSERT _Static_assert
#endif

// ~ Inlining
#if defined(_MSC_VER)
  #define _OGE_INLINE   __forceinline
  #define _OGE_NOINLINE __declspec(noinline)
#else // clang or gcc
  #define _OGE_INLINE   __attribute__((always_inline)) inline
  #define _OGE_NOINLINE __attribute__((noinline))
#endif

// ~ Utils
#define CLAMP(x, l, r) (((x) <= (l)) ? (l) : ((x) >= (r)) ? (r) : (x))

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define GIBIBYTES(x) ((x) * 1024ULL * 1024ULL * 1024ULL)
#define MEBIBYTES(x) ((x) * 1024ULL * 1024ULL)
#define KIBIBYTES(x) ((x) * 1024ULL)

#define GIGABYTES(x) ((x) * 1000ULL * 1000ULL * 1000ULL)
#define MEGABYTES(x) ((x) * 1000ULL * 1000ULL)
#define KILOBYTES(x) ((x) * 1000ULL)

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

