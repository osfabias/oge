#include "oge/defines.h"
#include "oge/core/memory.h"
#include "oge/core/events.h"
#include "oge/core/logging.h"
#include "oge/containers/darray.h"

#define MAX_EVENT_CODES 1024

struct {
  b8 initialized;
  OgeEventCallback* pDArrayCallbacks[MAX_EVENT_CODES];
} ogeEventsState = { .initialized = OGE_FALSE };

void ogeEventsInit() {
  if (ogeEventsState.initialized) {
    OGE_WARN("Trying to initialize events system while it's already initialized.");
    return;
  }

  ogeMemorySet(ogeEventsState.pDArrayCallbacks, 0, sizeof(ogeEventsState));
  ogeEventsState.initialized = OGE_TRUE;

  #ifdef OGE_DEBUG
  // Allocating so much darrays emits a lot of unwanted trace log
  // messages in debug build, so we are disabling trace messages
  const OgeLogLevel logLevel = ogeLoggingGetLevel();
  if (logLevel == OGE_LOG_LEVEL_TRACE) {
    ogeLoggingSetLevel(OGE_LOG_LEVEL_INFO);
  }
  #endif

  // Allocate DArray for each event code
  for(u16 i = 0; i < MAX_EVENT_CODES; ++i) {
    ogeEventsState.pDArrayCallbacks[i] = ogeDArrayAllocate(2, sizeof(OgeEventCallback));
  }

  #ifdef OGE_DEBUG
  ogeLoggingSetLevel(logLevel);
  OGE_TRACE("Allocated %d DArrays for event callbacks.", MAX_EVENT_CODES);
  #endif

  OGE_INFO("Events system initialized.");
}

void ogeEventsTerminate() {
  if (!ogeEventsState.initialized) {
    OGE_WARN("Trying to initialie events system while it'a already initialized.");
    return;
  }

  ogeEventsState.initialized = OGE_FALSE;

  #ifdef OGE_DEBUG
  // Deallocating darrays emits a lot of unwanted trace log
  // messages in debug build, so we are disabling trace messages
  const OgeLogLevel logLevel = ogeLoggingGetLevel();
  if (logLevel == OGE_LOG_LEVEL_TRACE) {
    ogeLoggingSetLevel(OGE_LOG_LEVEL_INFO);
  }
  #endif

  // Deallocate DArray for each event code
  for(u16 i = 0; i < MAX_EVENT_CODES; ++i) {
    ogeDArrayDeallocate(ogeEventsState.pDArrayCallbacks[i]);
  }

  #ifdef OGE_DEBUG
  ogeLoggingSetLevel(logLevel);
  OGE_TRACE("Deallocated %d DArrays for event callbacks.", MAX_EVENT_CODES);
  #endif

  OGE_INFO("Events system terminated.");
}

void ogeEventsSubscribe(u16 code, OgeEventCallback *pCallback) {
  ogeDArrayAppend(ogeEventsState.pDArrayCallbacks[code], pCallback);
}

void ogeEventsUnsubscribe(u16 code, OgeEventCallback *pCallback) {
  u64 index = ogeDArrayFind(ogeEventsState.pDArrayCallbacks[code], pCallback);

  if (index == -1) { return; }
  ogeDArrayRemove(ogeEventsState.pDArrayCallbacks[code], index);
}
