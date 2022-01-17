cmake_minimum_required(VERSION 3.12)

project(chicken3421)

set(CMAKE_CXX_STANDARD 17)

option(CHICKEN3421_INSTALL "Generate install target" ON)
set(GLFW_COMMIT_TAG master CACHE STRING "Which GLFW commit to use (for macOS compatibility")

message(STATUS "Using GLFW commit: ${GLFW_COMMIT_TAG}")

set(COMMON_LIBS glad glfw glm stb tinyobjloader)

add_library(chicken3421)
target_include_directories(
        chicken3421
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_sources(
        chicken3421
        PRIVATE
        include/chicken3421/chicken3421.hpp
        include/chicken3421/camera.hpp
        include/chicken3421/error_utils.hpp
        include/chicken3421/file_utils.hpp
        include/chicken3421/gl_utils.hpp
        include/chicken3421/window_utils.hpp

        src/camera.cpp
        src/error_utils.cpp
        src/file_utils.cpp
        src/gl_utils.cpp
        src/model.cpp
        src/window_utils.cpp
)
target_link_libraries(chicken3421 PUBLIC ${COMMON_LIBS})
