cmake_minimum_required(VERSION 3.18)
project(GLModelViewer)

set(CMAKE_CXX_STANDARD 20)
set(EXTERNAL_SOURCES )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

add_subdirectory(external/glfw-3.3.4)
add_subdirectory(external/kaitai_struct_cpp_stl_runtime)
find_package(Vulkan REQUIRED FATAL_ERROR)

set(EXTERNAL_SOURCES ${EXTERNAL_SOURCES} external/glad/src/glad.c external/stb/lib/stb_image.cpp)

include_directories(external/gl.hpp/include)
include_directories(external/glad/include)
include_directories(external/kaitai_struct_cpp_stl_runtime/kaitai)
include_directories(external/glfw-3.3.4/include)
include_directories(external/stb/include)
include_directories(external/glm)

include_directories(include)
set(HEADERS include/data/Mesh.h include/data/Shader.h include/data/VertexAttribute.h include/pch.h include/core/VulkanCore.hpp include/data/ShaderStage.h include/data/vk/CommandBuffer.h include/core/Events.h include/data/Texture.h include/data/TextureSampler.h include/data/RenderPipeline.h include/data/RenderPass.h include/data/Framebuffer.h include/core/VulkanDeviceObject.h include/core/Screen.h include/example/ExampleScreen.h include/ecs/Transform.h include/ecs/Entity.h include/ecs/Component.h include/ecs/components/CameraComponent.h include/ecs/Scene.h include/data/vk/FBO.h include/data/vk/Camera.h include/data/render/AbstractRenderer.h include/data/render/MeshRenderer.h include/data/Contexts.h include/ecs/components/MeshComponent.h include/ecs/components/MeshRendererComponent.h include/data/render/BlitRenderer.h)
set(SOURCES lib/data/Mesh.cpp lib/data/Shader.cpp lib/data/VertexAttribute.cpp lib/core/VulkanCore.cpp lib/data/ShaderStage.cpp lib/data/vk/CommandBuffer.cpp lib/data/Texture.cpp lib/data/TextureSampler.cpp lib/data/Framebuffer.cpp lib/data/RenderPass.cpp lib/core/Screen.cpp lib/example/ExampleScreen.cpp lib/ecs/Entity.cpp lib/ecs/components/CameraComponent.cpp lib/ecs/Scene.cpp lib/data/vk/FBO.cpp lib/data/vk/Camera.cpp lib/data/render/AbstractRenderer.cpp lib/data/render/MeshRenderer.cpp lib/data/Contexts.cpp lib/ecs/components/MeshComponent.cpp lib/ecs/components/MeshRendererComponent.cpp lib/data/render/BlitRenderer.cpp)

add_executable(GLModelViewer main.cpp ${SOURCES} ${HEADERS} ${EXTERNAL_SOURCES})

add_dependencies(GLModelViewer glfw)
add_dependencies(GLModelViewer kaitai_struct_cpp_stl_runtime)
target_link_directories(GLModelViewer PUBLIC ${CMAKE_BINARY_DIR}/external/glfw-3.3.4/src)
target_link_directories(GLModelViewer PUBLIC ${CMAKE_BINARY_DIR}/external/kaitai_struct_cpp_stl_runtime)
target_link_libraries(GLModelViewer glfw3)
target_link_libraries(GLModelViewer kaitai_struct_cpp_stl_runtime)
target_link_libraries(GLModelViewer Vulkan::Vulkan)

add_definitions("-DNOMINMAX")

include(cmake/VulkanTools.cmake)
target_add_shader(GLModelViewer sample/sample.vert)
target_add_shader(GLModelViewer sample/sample.frag)
target_add_shader(GLModelViewer sample_alt/sample.vert)
target_add_shader(GLModelViewer sample_alt/sample.frag)
target_add_shader(GLModelViewer blit/blit.vert)
target_add_shader(GLModelViewer blit/blit.frag)