project(oge VERSION 0.0.1)

# Setup "oge" library target
add_library(oge SHARED
            ./src/core/engine.c
            ./src/core/memory.c
            ./src/core/logging.c
            )
target_include_directories(oge PUBLIC include)
target_compile_definitions(oge PRIVATE OGE_EXPORT)

# Add platform source files
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  message(FATAL_ERROR "Windows platform now implemented.")
  # Add source files here instead of "message()" call
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "Linux platform not implemented.")
  # Add source files here instead of "message()" call
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin") # MACOS
  message(FATAL_ERROR "MacOS platform not implemented.")
  # Add source files here instead of "message()" call
elseif()
  message(FATAL_ERROR "Unknown platform.")
endif()

# Add compile definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(oge PRIVATE OGE_DEBUG)
else()
  target_compile_definitions(oge PRIVATE OGE_RELEASE)
endif()
