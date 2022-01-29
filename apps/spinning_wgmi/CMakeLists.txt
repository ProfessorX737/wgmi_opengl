# get base name of current directory to use as target name
get_filename_component(target ${CMAKE_CURRENT_LIST_DIR} NAME)

# set the output folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target})

add_executable(${target})

target_include_directories(${target} PUBLIC include)

target_sources(
        ${target}
        PRIVATE
        include/texture_2d.hpp
        include/shapes.hpp
        include/euler_camera.hpp
        include/memes.hpp
        include/mesh.hpp
        include/model.hpp
        include/scene.hpp
        include/renderer.hpp
        include/cubemap.hpp
        include/framebuffer.hpp

        src/main.cpp
        src/texture_2d.cpp
        src/shapes.cpp
        src/euler_camera.cpp
        src/memes.cpp
        src/mesh.cpp
        src/model.cpp
        src/renderer.cpp
        src/scene.cpp
        src/cubemap.cpp
        src/framebuffer.cpp
)

target_link_libraries(
        ${target}
        PUBLIC
        ${COMMON_LIBS}
)

copy_resources(${CMAKE_CURRENT_LIST_DIR}/res ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res)
