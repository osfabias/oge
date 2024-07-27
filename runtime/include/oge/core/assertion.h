/**
 * @file assertion.h
 * @brief The header of the assertion macro
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

#include "oge/core/logging.h"

#if _MSC_VER
  #include <intrin.h>
  #define debugBreak() __debugbreak()
#else
  #define debugBreak() __builtin_trap()
#endif

#ifdef OGE_DEBUG 
  /**
   * @brief Runtime assertion macro
   *
   * @param x A boolean expresion, that's asserted to be true.
   * @param msg A message to show on assertion fail.
   * @param ... VA arguments.
   */
  #define OGE_ASSERT(x, msg, ...) \
  if (!(x)) { OGE_FATAL(msg, ##__VA_ARGS__); debugBreak(); }
#else
  /**
   * @brief Runtime assertion macro
   *
   * @param x A boolean expresion, that's asserted to be true.
   * @param msg A message to show on assertion fail.
   * @param ... VA arguments.
   */
  #define OGE_ASSERT(x, msg, ...)
#endif
