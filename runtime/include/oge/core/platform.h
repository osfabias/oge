/**
 * @file platform.h
 * @brief The header of the platform layer
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

#include <vulkan/vulkan.h>

#include "oge/defines.h"

/**
 * @brief Platform layer initialization info.
 *
 * @var OplPlatformInitInfo::applicationName
 * Application name to be shown to user as a title
 * of the window or surface.
 *
 * @var OplPlatformInitInfo::width
 * A width of the window or surface in pixels.
 *
 * @var OplPlatformInitInfo::height
 * A height of the window or surface in pixels.
 */
typedef struct OgePlatformInitInfo {
  const char *applicationName;
  u16 width;
  u16 height;
} OgePlatformInitInfo;

/**
 * @brief Initializes platform layer.
 *
 * This function initializes platform window / surface manager and
 * creates a main window (on a desktop platforms) or a main surface
 * (on a mobile and console platforms).
 *
 * @return Returns OGE_TRUE if platform layer was successfully
 *         initialized or OGE_FALSE if not.
 */
b8 ogePlatformInit(const OgePlatformInitInfo *initInfo);

/**
 * @brief Terminates platform layer.
 */
void ogePlatformTerminate();

/**
 * @brief Pumps platform messages.
 */
#define ogePlatformPumpMessages() \
  oplPumpMessages()

/**
 * @brief Returns wheter platform requested application
 *        termination or not.
 * @return Returns OGE_TRUE if platform requested application
 *         termination or OGE_FALSE if not.
 */
b8 ogePlatformAppShouldClose();

/**
 * @brief Creates Vulkan surface.
 */
VkResult ogePlatformCreateSurface(
  VkInstance instance,
  const VkAllocationCallbacks *allocator,
  VkSurfaceKHR *surface);

/**
 * @brief Returns number of required extensions for Vulkan
 *        and their names.
 * @param extensionsCount A pointer to a variable that will hold
 *                        a number of required device extensions.
 * @param exntesionsNames A pointer to an array of strings that
 *                        will hold names of required device extensions.
 */
#define ogePlatformGetDeviceExtensions(extensionsCount, extensionNames) \
  oplGetDeviceExtensions(extensionsCount, extensionNames)
