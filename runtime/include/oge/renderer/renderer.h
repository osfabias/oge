/**
 * @file renderer.h
 * @brief The header of the renderer
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
 * @brief Renderer initialization info
 *
 * @var OgeRendererInitInfo::applicationName
 * An application name.
 *
 * @var OgeRendererInitInfo::applicationVersion
 * An application version made using OGE_MAKE_VERSION.
 */
typedef struct OgeRendererInitInfo {
  const char *applicationName;
  const u32 applicationVersion;
} OgeRendererInitInfo;

/**
 * @brief Initializes renderer.
 * @param initInfo A pointer to OgeRendererInitInfo struct.
 * @return Returns OGE_TRUE on successfull initialization, otherwise
 *         returns OGE_FALSE.
 */
b8 ogeRendererInit(const OgeRendererInitInfo *initInfo);

/**
 * @brief Terminates renderer.
 */
void ogeRendererTerminate();
