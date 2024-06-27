# Setup "oge" library target
file(GLOB_RECURSE OGE_RUNTIME_SOURCE_FILES ./src/oge/*)
add_library(oge SHARED ${OGE_RUNTIME_SOURCE_FILES})

target_include_directories(oge PUBLIC include)

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
  target_compile_definitions(oge PRIVATE _OGE_DEBUG)
else()
  target_compile_definitions(oge PRIVATE _OGE_RELEASE)
endif()
