cmake_minimum_required(VERSION 3.6)
project(tetris)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

find_package(OpenGL REQUIRED)
find_package(freetype REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLM REQUIRED glm)

include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})

set(SOURCE_FILES
    src/game.cpp
    src/tetris.h src/tetris.cpp
    src/render.h src/render.cpp
    src/util.h src/util.cpp
    src/stb_image.h)

add_executable(tetris ${SOURCE_FILES})
target_link_libraries(tetris ${OPENGL_gl_LIBRARY} ${GLFW_LIBRARIES} ${FREETYPE_LIBRARIES})
