#include <stdio.h>
#include <stdarg.h>

#include "oge/core/logging.h"
#include "oge/core/platform.h"

struct {
  OgeLogLevel logLevel;
} ogeLoggingState;

b8 ogeLoggingInit(const OgeLoggingInitInfo *pInitInfo) {
  ogeLoggingState.logLevel = pInitInfo->logLevel;

  // TODO: create log file
  return OGE_TRUE;
} 

void ogeLoggingTerminate() {
  // TODO: write all queued entries and close file
}

void ogeLog(OgeLogLevel level, const char *pMessage, ...) {
  if (level < ogeLoggingState.logLevel) { return; }

  // TODO: These string operations are all pretty slow. This needs to be
  // moved to another thread eventually, along with the file writes, to
  // avoid slowing things down while the engine is trying to run.
  const char* levelPrefixes[6] = {
    "TRACE\0", "INFO\0", "WARN\0", "ERROR\0", "FATAL\0",
  };

  // Technically imposes a 4k character limit on a single log entry, but...
  // DON'T DO THAT!
  char outMessage[4096];
  char formattedMessage[4096];
  ogePlatformMemorySet(outMessage, 0, sizeof(outMessage));
  ogePlatformMemorySet(formattedMessage, 0, sizeof(formattedMessage));

  // Format original message.
  // NOTE: Oddly enough, MS's headers override the GCC/Clang va_list type
  // with a "typedef char* va_list" in some cases, and as a result throws
  // a strange error here. The workaround for now is to just use __builtin_va_list,
  // which is the type GCC/Clang's va_start expects.
  __builtin_va_list valist;
  va_start(valist, pMessage);
  vsnprintf(formattedMessage, 4096, pMessage, valist);
  va_end(valist);

  // Prepend log level to message.
  sprintf(outMessage, "%s\t | %s\n", levelPrefixes[level], formattedMessage);

  // Pass along to console consumers.
  ogePlatformConsoleWrite(outMessage, level);
}
