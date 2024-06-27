#pragma once
/************************************************
 *                   macros                     *
 ************************************************/
// ~ DLL export
#ifdef _OGE_EXPORT
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

/************************************************
 *                  OGE types                   *
 ************************************************/
typedef struct OgeInitInfo {
  const char *pApplicationName;
  unsigned short width;
  unsigned short height;
} OgeInitInfo;

/************************************************
 *                 OGE tokens                   *
 ************************************************/
/** @brief True. */
#define OGE_TRUE 1

/** @brief False. */
#define OGE_FALSE 0

/**
 * Any id set to this should be considered invalid,
 * and not actually pointing to a real object.
 */
#define OGE_INVALID_ID_U64 18446744073709551615UL
#define OGE_INVALID_ID_U32 4294967295U
#define OGE_INVALID_ID_U16 65535U
#define OGE_INVALID_ID_U8  255U

/************************************************
 *                OGE public API                *
 ************************************************/
/* @brief Initializes OGE systems.
 *
 */
OGE_API int ogeInit(const OgeInitInfo *pInitInfo);

/* @brief Terminates OGE systems.
 *
 */
OGE_API void ogeTerminate();

