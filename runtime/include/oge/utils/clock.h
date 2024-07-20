#pragma once

#include "oge/defines.h"

typedef struct OgeClock {
  u64 startTime;
  u64 endTime;
  u64 elapsedTime;
} OgeClock; 

/**
 * @brief Sets start time to current OS time and resets elapsed time.
 */
void ogeClockStart(OgeClock *clock);

/**
 * @brief Sets stop time to current OS time and calculates elapsed time.
 */
void ogeClockStop(OgeClock *clock);
