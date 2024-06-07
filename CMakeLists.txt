cmake_minimum_required(VERSION 3.6)
project(tetris)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)

set(SOURCE_FILES
    src/game.cpp
    src/tetris.h src/tetris.cpp
    src/render.h src/render.cpp
    src/util.h src/util.cpp
    src/stb_image.h)

set(OpenGL_GL_PREFERENCE GLVND)
add_executable(tetris ${SOURCE_FILES})
target_link_libraries(tetris glfw glm::glm Freetype::Freetype OpenGL::GL GLEW::glew)
