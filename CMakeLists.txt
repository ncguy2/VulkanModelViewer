cmake_minimum_required(VERSION 3.18)
project(GLModelViewer)

set(CMAKE_CXX_STANDARD 20)
set(EXTERNAL_SOURCES )

add_subdirectory(external/glfw-3.3.4)

set(EXTERNAL_SOURCES ${EXTERNAL_SOURCES} external/glad/src/glad.c)

include_directories(external/gl.hpp/include)
include_directories(external/glad/include)
include_directories(external/glfw-3.3.4/include)

include_directories(include)
set(HEADERS include/display/Screen.h include/display/SimpleScreen.h include/data/Mesh.h include/data/Shader.h include/data/VertexAttribute.h include/pch.h)
set(SOURCES lib/display/Screen.cpp lib/display/SimpleScreen.cpp lib/data/Mesh.cpp lib/data/Shader.cpp lib/data/VertexAttribute.cpp)

add_executable(GLModelViewer main.cpp ${SOURCES} ${HEADERS} ${EXTERNAL_SOURCES})

add_dependencies(GLModelViewer glfw)
target_link_directories(GLModelViewer PUBLIC ${CMAKE_BINARY_DIR}/external/glfw-3.3.4/src)
target_link_libraries(GLModelViewer glfw3.lib)