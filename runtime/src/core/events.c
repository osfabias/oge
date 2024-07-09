#include "oge/core/assertion.h"
#include "oge/defines.h"
#include "oge/core/memory.h"
#include "oge/core/events.h"
#include "oge/core/logging.h"
#include "oge/containers/darray.h"

#define MAX_EVENT_CODES 1024

struct {
  b8 initialized;
  OgeEventCallback* pDArrayCallbacks[MAX_EVENT_CODES];
} s_eventsState = { .initialized = OGE_FALSE };

void ogeEventsInit() {
  if (s_eventsState.initialized) {
    OGE_WARN("Trying to initialize events system while it's already initialized.");
    return;
  }

  ogeMemorySet(s_eventsState.pDArrayCallbacks, 0, sizeof(s_eventsState));
  s_eventsState.initialized = OGE_TRUE;

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
    s_eventsState.pDArrayCallbacks[i] =
      ogeDArrayAllocate(2, sizeof(OgeEventCallback));
  }

  #ifdef OGE_DEBUG
  ogeLoggingSetLevel(logLevel);
  OGE_TRACE("Allocated %d DArrays for event callbacks.", MAX_EVENT_CODES);
  #endif

  OGE_INFO("Events system initialized.");
}

void ogeEventsTerminate() {
  if (!s_eventsState.initialized) {
    OGE_WARN("Trying to initialie events system while it'a already initialized.");
    return;
  }

  s_eventsState.initialized = OGE_FALSE;

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
    ogeDArrayDeallocate(s_eventsState.pDArrayCallbacks[i]);
  }

  #ifdef OGE_DEBUG
  ogeLoggingSetLevel(logLevel);
  OGE_TRACE("Deallocated %d DArrays for event callbacks.", MAX_EVENT_CODES);
  #endif

  OGE_INFO("Events system terminated.");
}

void ogeEventsSubscribe(u16 code, OgeEventCallback callback) {
  OGE_ASSERT(s_eventsState.initialized, "Trying to subscribe a callback to an event while events system is offline.");
  ogeDArrayAppend(s_eventsState.pDArrayCallbacks[code], &callback);
  OGE_TRACE("Subscribed %p callback for %d event.", &callback, code);
}

void ogeEventsUnsubscribe(u16 code, OgeEventCallback callback) {
  OGE_ASSERT(s_eventsState.initialized, "Trying to unsubscribe a callback from an event while events system is offline.");
  u64 index = ogeDArrayFind(s_eventsState.pDArrayCallbacks[code], &callback);

  if (index == -1) { return; }
  ogeDArrayRemove(s_eventsState.pDArrayCallbacks[code], index);
  OGE_TRACE("Unsubscribed %p callback for %d event.", &callback, code);
}

void ogeEventsInvoke(u16 code, void *sender, OgeEventData data) {
  const u64 callbacksCount = ogeDArrayLength(s_eventsState.pDArrayCallbacks[code]);
  for (u64 i = 0; i < callbacksCount; ++i) {
    const OgeEventCallback callback = s_eventsState.pDArrayCallbacks[code][i];

    // If event was processed - stop
    if(callback(sender, data)) { return; }
  }
}