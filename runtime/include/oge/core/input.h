/**
 * @file input.h
 * @brief The header of the input system 
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

#include <opl/opl.h>

#include "oge/defines.h"

/**
 * @brief Initialized input system.
 */
void ogeInputInit();

/**
 * @brief Terminates input system.
 */
void ogeInputTerminate();

/**
 * @brief Updates input system.
 * 
 * This function should be called at the end of a frame and
 * before pumping OPL messages.
 */
void ogeInputUpdate();

/**
 * @brief Returns wheter a key was pressed this frame or not.
 * @param key A key.
 * @return Returns OGE_TRUE if a key was pressed this frame,
 *         otherwise returns OGE_FALSE.
 */
OGE_API b8 ogeKeyIsPressed(OplKey key);

/**
 * @brief Returns wheter a key was down this frame or not.
 * @param key A key.
 * @return Returns OGE_TRUE if a key was down this frame,
 *         otherwise returns OGE_FALSE.
 */
OGE_API b8 ogeKeyIsDown(OplKey key);

/**
 * @brief Returns wheter a key was released this frame or not.
 * @param key A key.
 * @return Returns OGE_TRUE if a key was released this frame,
 *         otherwise returns OGE_FALSE.
 */
OGE_API b8 ogeKeyIsReleased(OplKey key);

/**
 * @brief Returns wheter a key was up this frame or not.
 * @param key A key.
 * @return Returns OGE_TRUE if a key was up this frame,
 *         otherwise returns OGE_FALSE.
 */
OGE_API b8 ogeKeyIsUp(OplKey key);

/**
 * @brief Returns wheter a mouseButton was pressed this frame or not.
 * @param mouseButton A key.
 * @return Returns OGE_TRUE if a mouseButton was pressed this frame,
 *         otherwise returns OGE_FALSE.
 */
OGE_API b8 ogeMouseButtonIsPressed(OplMouseButton mouseButton);

/**
 * @brief Returns wheter a mouseButton was down this frame or not.
 * @param mouseButton A key.
 * @return Returns OGE_TRUE if a mouseButton was down this frame,
 *         otherwise returns OGE_FALSE.
 */
OGE_API b8 ogeMouseButtonIsDown(OplMouseButton mouseButton);

/**
 * @brief Returns wheter a mouse button was released this frame or not.
 * @param mouseButton A key.
 * @return Returns OGE_TRUE if a mouse button was released this frame,
 *         otherwise returns OGE_FALSE.
 */
OGE_API b8 ogeMouseButtonIsReleased(OplMouseButton mouseButton);

/**
 * @brief Returns wheter a mouse button was up this frame or not.
 * @param mouseButton A key.
 * @return Returns OGE_TRUE if a mouse button was up this frame,
 *         otherwise returns OGE_FALSE.
 */
OGE_API b8 ogeMouseButtonIsUp(OplMouseButton mouseButton);

/**
 * @brief Returns a mouse wheel scroll.
 * @returns Returns a mouse wheel scroll.
 */
OGE_API i8 ogeMouseGetWheel();

/**
 * @brief Returns a current mouse position.
 * @param x A pointer to a variable that will hold x coordinate
 *          of a cursor.
 * @param y A pointer to a variable that will hold y coordinate
 *          of a cursor.
 */
OGE_API void ogeCursorGetPosition(u16 *x, u16 *y);

/**
 * @brief Retuns a difference between previous and current mouse position.
 * @param x A pointer to a variable that will hold x delta 
 *          of a cursor.
 * @param y A pointer to a variable that will hold y delta 
 *          of a cursor.
 */
OGE_API void ogeCursorGetDelta(u16 *x, u16 *y);

