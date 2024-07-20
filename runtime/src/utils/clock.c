#include "oge/utils/clock.h"
#include "oge/core/platform.h"

void ogeClockStart(OgeClock *clock) {
  clock->startTime = ogePlatformGetTime();
  clock->elapsedTime = 0;
  clock->endTime = clock->startTime;
}

void ogeClockStop(OgeClock *clock) {
  clock->endTime = ogePlatformGetTime();
  clock->elapsedTime = clock->endTime - clock->startTime;
}
