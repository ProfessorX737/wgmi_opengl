cmake_minimum_required(VERSION 3.12)

project(COMP3421-Setup VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

include(copy_resources)

add_subdirectory(lib)
set(COMMON_LIBS glfw glm glad stb chicken3421)
add_subdirectory(apps)
