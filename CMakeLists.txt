cmake_minimum_required(VERSION 3.10)
project(mygame C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_C_STANDARD 11)

add_subdirectory(external/glad)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories(
    include
    external/glad/include
    ${GLFW_INCLUDE_DIRS}
)

add_executable(mygame
    src/camera.c
    src/collitions.c
    src/main.c
    src/engine.c
    src/renderer.c
    src/transform.c
    src/object.c
)

find_package(cglm REQUIRED)

target_link_libraries(mygame PRIVATE
    cglm
    glad
    ${GLFW_LIBRARIES}
    dl
    m
)
