project(oge VERSION 0.0.1)

# Find packages
find_package(Vulkan REQUIRED)

# Setup "oge" library target
add_library(oge SHARED
  ./src/core/input.c
  ./src/core/engine.c
  ./src/core/memory.c
  ./src/core/events.c
  ./src/core/logging.c

  ./src/renderer/renderer.c

  ./src/utils/clock.c

  ./src/containers/darray.c
  )
target_include_directories(oge PUBLIC include)
target_compile_definitions(oge PRIVATE
  OGE_EXPORT
  OGE_VERSION_MAJOR=${OGE_VERSION_MAJOR}
  OGE_VERSION_MINOR=${OGE_VERSION_MINOR}
  OGE_VERSION_PATCH=${OGE_VERSION_PATCH}
)
target_link_libraries(oge PUBLIC Vulkan::Vulkan)

# Add platform source files
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  message(FATAL_ERROR "Windows platform not implemented.")
  # Add source files here instead of "message()" call
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "Linux platform not implemented.")
  # Add source files here instead of "message()" call
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin") # MACOS
  target_sources(oge PRIVATE
    ./src/platform/platform_macos.m
  )

  find_library(COCOA_LIBRARY Cocoa)
  find_library(QUARTZ_LIBRARY QuartzCore)
  target_link_libraries(oge PUBLIC
    ${COCOA_LIBRARY}
    ${QUARTZ_LIBRARY}
  )

  target_compile_definitions(oge PUBLIC
    OGE_METAL_ENABLED
    OGE_VULKAN_ENABLED
  )
elseif()
  message(FATAL_ERROR "Unknown platform.")
endif()

# Add compile definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(oge PRIVATE OGE_DEBUG)
else()
  target_compile_definitions(oge PRIVATE OGE_RELEASE)
endif()
