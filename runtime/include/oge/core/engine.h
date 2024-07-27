/**
 * @file engine.h
 * @brief The header of the main OGE functions
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

#include <opl/opl.h>

#include "oge/defines.h"
#include "oge/core/logging.h"
#include "oge/renderer/renderer.h"

// forward decl for struct from oge/core/application.h
typedef struct OgeApplication OgeApplication;

/** 
 * @brief OGE initialization info.
 *
 * @var OgeInitInfo::oplInitInfo
 * A pointer to a OplInitInfo struct.
 *
 * @var OgeInitInfo::loggingInitInfo
 * A pointer to a OgeLoggingInitInfo struct.
 *
 * @var OgeInitInfo ::rendererInitInfo
 * A pointer to a OgeRendererInitInfo struct.
 */
typedef struct OgeInitInfo {
  const OplInitInfo         *oplInitInfo;
  const OgeLoggingInitInfo  *loggingInitInfo;
  const OgeRendererInitInfo *rendererInitInfo;
} OgeInitInfo;

/**
 *  @brief Initializes OGE.
 *  @param application A pointer to the OgeApplication struct.
 *  @return If OGE was initialized successfully it returns OGE_TRUE,
 *          otherwise it returns OGE_FALSE.
 */
OGE_API b8 ogeInit(const OgeApplication *application);

/**
 * @brief Starts main application cycle.
 */
OGE_API void ogeRun();

/**
 * @brief Terminates OGE.
 */
OGE_API void ogeTerminate();
