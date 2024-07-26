#include <opl/opl.h>

#include "oge/utils/clock.h"

void ogeClockStart(OgeClock *clock) {
  clock->startTime = oplGetTime();
  clock->elapsedTime = 0;
  clock->endTime = clock->startTime;
}

void ogeClockStop(OgeClock *clock) {
  clock->endTime = oplGetTime();
  clock->elapsedTime = clock->endTime - clock->startTime;
}
