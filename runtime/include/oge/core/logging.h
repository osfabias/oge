/**
 * @file logging.h
 * @brief The header of the logging system 
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
 * @brief Logging level.
 *
 * Logging level specifies the minimum level of messages
 * to show.
 */
typedef enum OgeLogLevel {
  OGE_LOG_LEVEL_TRACE,
  OGE_LOG_LEVEL_INFO,
  OGE_LOG_LEVEL_WARN,
  OGE_LOG_LEVEL_ERROR,
  OGE_LOG_LEVEL_FATAL,
} OgeLogLevel;

/**
 * @brief Logging system initialization info.
 *
 * @var fileName A name of a file to write logs to.
 * @vat logLevel A logging level.
 */
typedef struct OgeLoggingInitInfo {
  const char *fileName;
  OgeLogLevel logLevel;
} OgeLoggingInitInfo;

/**
 * @brief Initializes logging system.
 * @param initInfo A pointer to OgeLoggingInitInfo struct.
 * @return Returns OGE_TRUE is logging system was successfully
 *         initialized, otherwise returns OGE_FALSE.
 */
b8 ogeLoggingInit(const OgeLoggingInitInfo *initInfo);

/**
 * @brief Terminates logging system.
 */
void ogeLoggingTerminate();

/**
 * @brief Ptrints a log message with given logging level.
 *
 * If logging level is lower than current set logging level,
 * message wouldn't be displayed.
 *
 * @param level A logging level.
 * @param msg A message to print.
 * @param ... VA arguments.
 */
OGE_API void ogeLog(OgeLogLevel level, const char *msg, ...);

#ifdef OGE_DEBUG
  /**
   * @brief Prints trace log message.
   * @param msg A message to print.
   * @param ... VA arguments.
   */
  #define OGE_TRACE(msg, ...) ogeLog(OGE_LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
  /**
   * @brief Prints trace log message.
   * @param msg A message to print.
   * @param ... VA arguments.
   */
  #define OGE_TRACE(msg, ...)
#endif

/**
 * @brief Prints info log message.
 * @param msg A message to print.
 * @param ... VA arguments.
 */
#define OGE_INFO(msg, ...)  ogeLog(OGE_LOG_LEVEL_INFO,  msg, ##__VA_ARGS__)

/**
 * @brief Prints warn log message.
 * @param msg A message to print.
 * @param ... VA arguments.
 */
#define OGE_WARN(msg, ...)  ogeLog(OGE_LOG_LEVEL_WARN,  msg, ##__VA_ARGS__)

/**
 * @brief Prints error log message.
 * @param msg A message to print.
 * @param ... VA arguments.
 */
#define OGE_ERROR(msg, ...) ogeLog(OGE_LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)

/**
 * @brief Prints fatal log message.
 * @param msg A message to print.
 * @param ... VA arguments.
 */
#define OGE_FATAL(msg, ...) ogeLog(OGE_LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)
