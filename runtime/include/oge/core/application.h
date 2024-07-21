#pragma once

#include "oge/core/engine.h"

typedef struct OgeApplication OgeApplication;

/** @struct OgeApplication
 *  @brief OGE application state struct.
 *
 *  This struct contains OGE initialization info, pointers to 
 *  application functions and a pointer to user defined internal
 *  app state.
 *
 *  @var OgeApplication::pOgeInitInfo
 *  A pointer to the OgeInitInfo struct.
 *
 *  @var OgeApplication::pInitFunction
 *  A pointer to the application initialization function. This function
 *  is called after the initialization of all OGE systems.
 *
 *  @var OgeApplication::pUpdateFunction
 *  A pointer to the application update function. This function
 *  is called after pumping platform messages.
 *
 *  @var OgeApplication::pRenderFunction
 *  A pointer to the application render function. This function
 *  is called after the application update function.
 *
 *  @var OgeApplication::pTerminateFunction
 *  A pointer to the application terminate function.
 *  This function is called before the OGE system termination.
 *
 *  @var OgeApplication::pState
 *  A pointer to the application internal state.
 */
struct OgeApplication {
  OgeInitInfo *pOgeInitInfo;

  b8   (*pfnInit)(const OgeApplication *pApplication);
  b8   (*pfnUpdate)(const OgeApplication *pApplication);
  b8   (*pfnRender)(const OgeApplication *pApplication);
  void (*pfnTerminate)(const OgeApplication *pApplication);

  void *pState;
};
