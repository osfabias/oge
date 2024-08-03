message(STATUS "Configuring OGE runtime...")

# ~ find packages
find_package(Vulkan REQUIRED)

# ~ target setup
add_library(oge SHARED
  ./src/core/input.c
  ./src/core/engine.c
  ./src/core/memory.c
  ./src/core/events.c
  ./src/core/logging.c
  ./src/core/platform.c

  ./src/renderer/renderer.c

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

# ~ configure dependencies 
add_subdirectory(deps)
