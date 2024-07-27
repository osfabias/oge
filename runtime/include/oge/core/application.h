/**
 * @file application.h
 * @brief The header of the OgeApplication struct
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

#include "oge/core/engine.h"

typedef struct OgeApplication OgeApplication;

/**
 * @brief OGE application struct
 *
 * This struct contains OGE initialization info, pointers to 
 * an application functions and a pointer to user defined
 * internal application state.
 *
 * @var OgeApplication::ogeInitInfo
 * A pointer to the OgeInitInfo struct.
 *
 * @var OgeApplication::init
 * A pointer to the application initialization function. This function
 * is called after the initialization of all OGE systems.
 *
 * @var OgeApplication::update
 * A pointer to the application update function. This function
 * is called after pumping OPL messages.
 *
 * @var OgeApplication::render
 * A pointer to the application render function. This function
 * is called after the application update function.
 *
 * @var OgeApplication::terminate
 * A pointer to the application terminate function.
 * This function is called before the OGE system termination.
 */
struct OgeApplication {
  const OgeInitInfo *ogeInitInfo;

  b8   (*init)      ();
  b8   (*update)    ();
  b8   (*render)    ();
  void (*terminate) ();
};
