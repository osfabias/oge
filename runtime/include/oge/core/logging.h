#pragma once

#include "oge/defines.h"

typedef enum OgeLogLevel {
  OGE_LOG_LEVEL_TRACE,
  OGE_LOG_LEVEL_INFO,
  OGE_LOG_LEVEL_WARN,
  OGE_LOG_LEVEL_ERROR,
  OGE_LOG_LEVEL_FATAL,
} OgeLogLevel;

typedef struct OgeLoggingInitInfo {
  const char *pFileName;
  OgeLogLevel logLevel;
} OgeLoggingInitInfo;

b8   ogeLoggingInit(const OgeLoggingInitInfo *pInitInfo);
void ogeLoggingTerminate();
void ogeLoggingSetLevel(OgeLogLevel level);

OGE_API void ogeLog(OgeLogLevel level, const char *pMessage, ...);

#ifdef OGE_DEBUG
  #define OGE_TRACE(msg, ...) ogeLog(OGE_LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
  #define OGE_TRACE(msg, ...)
#endif

#define OGE_INFO(msg, ...)  ogeLog(OGE_LOG_LEVEL_INFO,  msg, ##__VA_ARGS__)
#define OGE_WARN(msg, ...)  ogeLog(OGE_LOG_LEVEL_WARN,  msg, ##__VA_ARGS__)
#define OGE_ERROR(msg, ...) ogeLog(OGE_LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
#define OGE_FATAL(msg, ...) ogeLog(OGE_LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)
