#include <stdio.h>
#include <stdarg.h>

#include <opl/opl.h>

#include "oge/core/memory.h"
#include "oge/core/logging.h"
#include "oge/core/assertion.h"

static struct {
  b8 initialized;
  OgeLogLevel logLevel;
} s_loggingState = { .initialized = OGE_FALSE };

b8 ogeLoggingInit(const OgeLoggingInitInfo *pInitInfo) {
  OGE_ASSERT(
    !s_loggingState.initialized,
    "Trying to initialize logging system while it's already initialized."
  );

  // TODO: create log file

  s_loggingState.initialized = OGE_TRUE;
  s_loggingState.logLevel    = pInitInfo->logLevel;

  OGE_INFO("Logging system initialized.");

  return OGE_TRUE;
} 

void ogeLoggingTerminate() {
  OGE_ASSERT(
    s_loggingState.initialized,
    "Trying to terminate logging system while it's already terminated."
  );

  s_loggingState.initialized = OGE_FALSE;

  // TODO: write all queued entries and close file

  OGE_INFO("Logging system terminated.");
}

void ogeLog(OgeLogLevel level, const char *msg, ...) {
  if (level < s_loggingState.logLevel) { return; }

  // TODO: These string operations are all pretty slow. This needs to be
  // moved to another thread eventually, along with the file writes, to
  // avoid slowing things down while the engine is trying to run.
  const char* levelPrefixes[6] = {
    "TRACE ", "INFO  ", "WARN  ", "ERROR ", "FATAL ",
  };

  // Technically imposes a 4k character limit on a single
  // log entry, but... DON'T DO THAT!
  char outMessage[4096];
  char formattedMessage[4096];
  ogeMemSet(outMessage, 0, sizeof(outMessage));
  ogeMemSet(formattedMessage, 0, sizeof(formattedMessage));

  // Format original message.
  // NOTE: Oddly enough, MS's headers override the GCC/Clang va_list type
  // with a "typedef char* va_list" in some cases, and as a result throws
  // a strange error here. The workaround for now is to just use
  // __builtin_va_list, which is the type GCC/Clang's va_start expects.
  __builtin_va_list valist;
  va_start(valist, msg);
  vsnprintf(formattedMessage, 4096, msg, valist);
  va_end(valist);

  // Prepend log level to message.
  sprintf(outMessage, "%s | %s\n", levelPrefixes[level], formattedMessage);

  // Pass along to console consumers.
  oplConsoleWrite(outMessage, 1 + level);
}
