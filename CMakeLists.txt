cmake_minimum_required(VERSION 3.16)

#####################################################
#                   project setup                   #
#####################################################
# ~ version
set(OGE_VERSION_MAJOR 0)
set(OGE_VERSION_MINOR 0)
set(OGE_VERSION_PATCH 1)
set(OGE_VERSION ${OGE_VERSION_MAJOR}.${OGE_VERSION_MINOR}.${OGE_VERSION_PATCH})

# ~ project creation
project(oge
  VERSION ${OGE_VERSION_MAJOR}.${OGE_VERSION_MINOR}.${OGE_VERSION_PATCH}
  LANGUAGES C
)

# ~ cmake configration
set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ~ options
option(OGE_BUILD_EXAMPLE "Build example project." ON)
option(OGE_BUILD_TESTS "Build tests." ON)

# ~ printing info
message(STATUS "==== OGE info ====")
message(STATUS "version: ${OGE_VERSION}")
message(STATUS "OGE_BUILD_EXAMPLE: ${OGE_BUILD_EXAMPLE}")
message(STATUS "OGE_BUILD_TESTS: ${OGE_BUILD_EXAMPLE}")

# ~ adding subdirs
add_subdirectory(runtime)

if (OGE_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()

if (OGE_BUILD_TESTS)
  #add_subdirectory(tests)
endif()
