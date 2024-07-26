#pragma once

#include "opl/opl.h"

#include "oge/defines.h"
#include "oge/core/logging.h"
#include "oge/renderer/renderer.h"

// forward decl for struct from oge/core/application.h
typedef struct OgeApplication OgeApplication;

typedef struct OgeInitInfo OgeInitInfo;

/** @struct OgeInitInfo
 *  @brief Struct that contains OGE initialization info.
 *
 *  @var OgeInitInfo::pLoggingInitInfo
 *  Member pLoggingInitInfo is a pointer to the OGE logging
 *  system initialization info.
 *
 *  @var OgePlatformInitInfo::pPlatformInitInfo
 *  Member pPlatformInitInfo is a pointer to the OGE platform
 *  initialization info.
 */
struct OgeInitInfo {
  OplInitInfo *pOplInitInfo;
  OgeLoggingInitInfo  *pLoggingInitInfo;
  OgeRendererInitInfo *pRendererInitInfo;
};

/**
 *  @brief Initializes OGE systems.
 *
 *  @param pApplication A pointer to the application state variable.
 *
 *  @return OGE_TRUE on succeed, else OGE_FALSE.
 */
OGE_API b8 ogeInit(const OgeApplication *pApplication);

/**
 *  @brief Starts main application cycle.
 */
OGE_API void ogeRun();

/**
 *  @brief Terminates OGE systems.
 */
OGE_API void ogeTerminate();
