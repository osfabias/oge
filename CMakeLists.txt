cmake_minimum_required(VERSION 3.16)

project(oge VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(runtime)
add_subdirectory(example)
