project(oge VERSION 0.0.1)

# ~ build libraries
add_subdirectory(lib)

# ~ find packages
find_package(Vulkan REQUIRED)

# ~ target setup
add_library(oge SHARED
  ./src/core/input.c
  ./src/core/engine.c
  ./src/core/memory.c
  ./src/core/events.c
  ./src/core/logging.c

  ./src/renderer/debug.c
  ./src/renderer/querries.c
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
target_link_libraries(oge PUBLIC opl Vulkan::Vulkan)

# ~ add compile definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(oge PRIVATE OGE_DEBUG)
else()
  target_compile_definitions(oge PRIVATE OGE_RELEASE)
endif()
