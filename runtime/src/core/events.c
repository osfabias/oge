#include "oge/defines.h"
#include "oge/core/memory.h"
#include "oge/core/events.h"
#include "oge/core/logging.h"
#include "oge/core/assertion.h"
#include "oge/containers/darray.h"

#define MAX_EVENT_CODES 1024

struct {
  b8 initialized;
  OgeEventCallback* callbacks[MAX_EVENT_CODES]; // darrays
} s_eventsState = { .initialized = OGE_FALSE };

void ogeEventsInit() {
  if (s_eventsState.initialized) {
    OGE_WARN("Trying to initialize events system while it's already initialized.");
    return;
  }

  ogeMemSet(s_eventsState.callbacks, 0, sizeof(s_eventsState));
  s_eventsState.initialized = OGE_TRUE;

  // Allocate DArray for each event code
  for(u16 i = 0; i < MAX_EVENT_CODES; ++i) {
    s_eventsState.callbacks[i] = ogeDArrayAlloc(2, sizeof(OgeEventCallback));
  }

  OGE_INFO("Events system initialized.");
}

void ogeEventsTerminate() {
  if (!s_eventsState.initialized) {
    OGE_WARN("Trying to initialie events system while it'a already initialized.");
    return;
  }

  s_eventsState.initialized = OGE_FALSE;

  // Deallocate DArray for each event code
  for(u16 i = 0; i < MAX_EVENT_CODES; ++i) {
    ogeDArrayFree(s_eventsState.callbacks[i]);
  }

  OGE_INFO("Events system terminated.");
}

void ogeEventsSubscribe(u16 code, OgeEventCallback callback) {
  OGE_ASSERT(s_eventsState.initialized, "Trying to subscribe a callback to an event while events system is offline.");
  ogeDArrayAppend(s_eventsState.callbacks[code], &callback);
  OGE_TRACE("Subscribed %p callback for %d event.", &callback, code);
}

void ogeEventsUnsubscribe(u16 code, OgeEventCallback callback) {
  OGE_ASSERT(s_eventsState.initialized, "Trying to unsubscribe a callback from an event while events system is offline.");
  u64 index = ogeDArrayFind(s_eventsState.callbacks[code], &callback);

  if (index == -1) { return; }
  ogeDArrayRemove(s_eventsState.callbacks[code], index);
  OGE_TRACE("Unsubscribed %p callback for %d event.", &callback, code);
}

void ogeEventsInvoke(u16 code, void *invoker, OgeEventData data) {
  const u64 callbacksCount =
    ogeDArrayLength(s_eventsState.callbacks[code]);
  for (u64 i = 0; i < callbacksCount; ++i) {
    const OgeEventCallback callback = s_eventsState.callbacks[code][i];

    // If event was processed - stop
    if(callback(invoker, data)) { return; }
  }
}
