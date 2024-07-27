/**
 * @file clock.h
 * @brief The header of the clock util
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
 * @var OgeClock::startTime
 * A number of milliseconds past since the Unix epoch at the 
 * moment of ogeClockStart() call.
 *
 * @var OgeClock::endTime
 * A number of milliseconds past since the Unix epoch at the
 * moment of ogeClockStop() call.
 *
 * @var OgeClock::elapsedTime
 * A difference between startTime and endTime.
 */
typedef struct OgeClock {
  u64 startTime;
  u64 endTime;
  u64 elapsedTime;
} OgeClock; 

/**
 * Sets the start time to current OS time and resets the elapsed time.
 *
 * @param clock A pointer to the OgeClock structure.
 */
void ogeClockStart(OgeClock *clock);

/**
 * Sets the stop time to current OS time and calculates the elapsed time.
 *
 * @param clock A pointer to the OgeClock structure.
 */
void ogeClockStop(OgeClock *clock);
