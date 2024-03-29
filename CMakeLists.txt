cmake_minimum_required(VERSION 3.6)
project(tetris)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

find_package(OpenGL REQUIRED)
find_package(freetype REQUIRED)

find_package(PkgConfig REQUIRED)
if(NOT APPLE)
    pkg_search_module(GLFW REQUIRED glfw3)
    pkg_search_module(GLM REQUIRED glm)
endif(APPLE)

include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})

# pkg_search_module doesn't provide a full path to a library,
# without with line xcode project (for example) won't build.
link_directories(${GLFW_LIBRARY_DIRS})

set(SOURCE_FILES
    src/game.cpp
    src/tetris.h src/tetris.cpp
    src/render.h src/render.cpp
    src/util.h src/util.cpp
    src/stb_image.h)

add_executable(tetris ${SOURCE_FILES})
target_link_libraries(tetris ${OPENGL_gl_LIBRARY} ${GLFW_LIBRARIES} ${FREETYPE_LIBRARIES})

if(APPLE)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED) 
   	target_link_libraries(tetris glfw)
	target_link_libraries(tetris glm::glm)
endif(APPLE)

