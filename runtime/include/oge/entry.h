#pragma once

#include "oge/core/engine.h"
#include "oge/core/logging.h"
#include "oge/core/assertion.h"
#include "oge/core/application.h"
#include "oge/core/memory.h"

/**
 * @brief Fills an application struct.
 * 
 * Must be defined by user.
 *
 */
extern b8 ogeApplicationCreate(OgeApplication *pApplication);

/**
 * @brief OGE application entry point.
 */
int main() {
  OgeApplication ogeApplication;

  ogeMemoryInit();

  if (!ogeApplicationCreate(&ogeApplication)) {
    OGE_FATAL("Failed to create OGE application");
    return 1;
  }

  if(!ogeInit(&ogeApplication)) {
    OGE_FATAL("Failed to initialize OGE");
    return 1;
  }

  ogeRun();

  ogeTerminate();

  ogeMemoryTerminate();
  return 0;
}
