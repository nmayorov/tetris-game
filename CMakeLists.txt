cmake_minimum_required(VERSION 3.6)
project(tetris)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

find_package(OpenGL REQUIRED)
include_directories(/usr/local/include/freetype2/)

set(SOURCE_FILES
    src/game.cpp
    src/tetris.h src/tetris.cpp
    src/render.h src/render.cpp
    src/util.h src/util.cpp
    src/stb_image.h)

add_executable(tetris ${SOURCE_FILES})
target_link_libraries(tetris ${OPENGL_gl_LIBRARY} glfw freetype)
