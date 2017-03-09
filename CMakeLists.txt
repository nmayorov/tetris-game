cmake_minimum_required(VERSION 3.6)
project(tetris)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

find_package(OpenGL REQUIRED)
include_directories(/usr/local/include/freetype2/)

set(SOURCE_FILES
    tetris/game.cpp
    tetris/tetris.h tetris/tetris.cpp
    tetris/render.h tetris/render.cpp 
    tetris/util.h tetris/util.cpp
    tetris/stb_image.h)

add_executable(tetris ${SOURCE_FILES})
target_link_libraries(tetris ${OPENGL_gl_LIBRARY} glfw freetype)
