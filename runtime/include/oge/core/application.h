#pragma once

#include "oge/core/engine.h"

typedef struct OgeApplication OgeApplication;

/** @struct OgeApplication
 *  @brief OGE application state struct.
 *
 *  This struct contains OGE initialization info, application
 *  functions and a pointer to user defined internal app state.
 *
 *  @var OgeApplication::pOgeInitInfo
 *  Member pOgeInitInfo contains a pointer to the OgeInitInfo struct.
 *
 *  @var OgeApplication::pInitFunction
 *  Member pInitFunction is a pointer to the application
 *  initialization function. This function is called after the
 *  initialization of all OGE systems.
 *
 *  @var OgeApplication::pUpdateFunction
 *  Member pUpdateFunction is a pointer to the application
 *  update function. This function is called after pumping platform
 *  messages.
 *
 *  @var OgeApplication::pRenderFunction
 *  Member pRenderFunction is a pointer to the application
 *  render function. This function is called after the application
 *  update function.
 *
 *  @var OgeApplication::pTerminateFunction
 *  Member pTerminateFunction is a pointer to the application
 *  terminate function. This function is called before the OGE
 *  system termination.
 *
 *  @var OgeApplication::pState
 *  Member pState is a pointer to the application internal state.
 */
struct OgeApplication {
  OgeInitInfo *pOgeInitInfo;

  b8   (*pInitFunction)(const OgeApplication *pApplication);
  b8   (*pUpdateFunction)(const OgeApplication *pApplication);
  b8   (*pRenderFunction)(const OgeApplication *pApplication);
  void (*pTerminateFunction)(const OgeApplication *pApplication);

  void *pState;
};
