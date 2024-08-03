message(STATUS "Configuring OGE example project...")

# ~ add executable
add_executable(example main.c)

file(COPY shaders DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_link_libraries(example PRIVATE oge)

# ~ build shaders
message(STATUS "Building shaders...")
set(SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
execute_process(COMMAND mkdir ${SHADERS_DIR}/spir-v)
execute_process(COMMAND glslc ${SHADERS_DIR}/glsl/default.frag -o ${SHADERS_DIR}/spir-v/main-frag.spv)
execute_process(COMMAND glslc ${SHADERS_DIR}/glsl/default.vert -o ${SHADERS_DIR}/spir-v/main-vert.spv)
