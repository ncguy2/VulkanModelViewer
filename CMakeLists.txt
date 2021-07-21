cmake_minimum_required(VERSION 3.18)
project(GLModelViewer)

include(cmake/VulkanTools.cmake)

set(CMAKE_CXX_STANDARD 20)
set(EXTERNAL_SOURCES )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(ENABLED_PLUGINS "PMX;STDTEX")
option(BUILD_LIB "Whether to build the viewer as a dll" ON)

message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

add_subdirectory(external/glfw)
add_subdirectory(external/kaitai_struct_cpp_stl_runtime)
#add_subdirectory(external/harfbuzz)
#add_subdirectory(external/freetype)
find_package(Vulkan REQUIRED FATAL_ERROR)

set(EXTERNAL_SOURCES ${EXTERNAL_SOURCES} external/glad/src/glad.c external/stb/lib/stb_image.cpp)

include_directories(external/glad/include)
include_directories(external/kaitai_struct_cpp_stl_runtime)
include_directories(external/glfw/include)
include_directories(external/stb/include)
include_directories(external/glm)
#include_directories(external/freetype/include)
include_directories(include)

#set(PLUGIN_HEADERS )
#set(PLUGIN_SOURCES )
set(PLUGIN_HEADER_ROOT ${CMAKE_SOURCE_DIR}/include/plugins/api)

set(PLATFORM_HEADERS )
set(PLATFORM_SOURCES )

if(WIN32)
    set(WIN32_HEADERS include/platform/win32/WindowHost.h)
    set(WIN32_SOURCES lib/platform/win32/WindowHost.cpp)

    message("Adding Win32 sources")

    set(PLATFORM_HEADERS ${PLATFORM_HEADERS} ${WIN32_HEADERS})
    set(PLATFORM_SOURCES ${PLATFORM_SOURCES} ${WIN32_SOURCES})
endif(WIN32)


set(HEADERS include/data/Mesh.h include/data/Shader.h include/data/VertexAttribute.h include/pch.h include/core/VulkanCore.hpp include/data/ShaderStage.h include/data/vk/CommandBuffer.h include/core/Events.h include/data/Texture.h include/data/TextureSampler.h include/data/RenderPipeline.h include/data/RenderPass.h include/data/Framebuffer.h include/core/VulkanDeviceObject.h include/core/Screen.h include/example/ExampleScreen.h include/ecs/Transform.h include/ecs/Entity.h include/ecs/Component.h include/ecs/components/CameraComponent.h include/ecs/Scene.h include/data/vk/FBO.h include/data/vk/Camera.h include/data/render/AbstractRenderer.h include/data/render/MeshRenderer.h include/data/Contexts.h include/ecs/components/MeshComponent.h include/ecs/components/MeshRendererComponent.h include/scene2d/Stage.h include/scene2d/Actor.h include/scene2d/Group.h include/scene2d/Scene2d.h include/scene2d/Event.h include/scene2d/EventListener.h include/plugins/api/Plugin.h include/plugins/PluginHost.h include/data/MeshData.h include/plugins/PluginManager.h include/data/TextureAtlas.h include/data/render/GridRenderer.h include/data/render/RendererStack.h include/data/VertexDescription.h include/ecs/components/FlyCamComponent.h include/ecs/components/NameComponent.h include/startup.h include/Logging.h include/core/ConfigRegistry.h include/core/Utils.h include/data/DataSmoother.h include/data/LimitedQueue.h include/interop/InteropHost.h include/interop/MetadataRecord.h include/interop/MetadataSet.h)
set(SOURCES lib/data/Mesh.cpp lib/data/Shader.cpp lib/data/VertexAttribute.cpp lib/core/VulkanCore.cpp lib/data/ShaderStage.cpp lib/data/vk/CommandBuffer.cpp lib/data/Texture.cpp lib/data/TextureSampler.cpp lib/data/Framebuffer.cpp lib/data/RenderPass.cpp lib/core/Screen.cpp lib/example/ExampleScreen.cpp lib/ecs/Entity.cpp lib/ecs/components/CameraComponent.cpp lib/ecs/Scene.cpp lib/data/vk/FBO.cpp lib/data/vk/Camera.cpp lib/data/render/AbstractRenderer.cpp lib/data/render/MeshRenderer.cpp lib/data/Contexts.cpp lib/ecs/components/MeshComponent.cpp lib/ecs/components/MeshRendererComponent.cpp lib/scene2d/Event.cpp lib/plugins/PluginHost.cpp lib/plugins/PluginManager.cpp lib/data/TextureAtlas.cpp lib/data/render/GridRenderer.cpp lib/data/render/RendererStack.cpp lib/data/VertexDescription.cpp lib/data/render/FlyCamComponent.cpp lib/startup.cpp lib/Logging.cpp lib/core/ConfigRegistry.cpp lib/core/Utils.cpp lib/interop/InteropHost.cpp lib/interop/MetadataSet.cpp)

if(BUILD_LIB)
    add_library(GLModelViewer SHARED ${SOURCES} ${HEADERS} ${EXTERNAL_SOURCES} ${PLATFORM_HEADERS} ${PLATFORM_SOURCES})
else()
    add_executable(GLModelViewer main.cpp ${SOURCES} ${HEADERS} ${EXTERNAL_SOURCES})
endif()

if("STDTEX" IN_LIST ENABLED_PLUGINS)
    add_plugin(GLModelViewer "plugins/StandardTextures" StdTexPlugin)
endif()
if("PMX" IN_LIST ENABLED_PLUGINS)
    add_plugin(GLModelViewer "plugins/PMX" PMXPlugin)
endif()

add_dependencies(GLModelViewer glfw)
#add_dependencies(GLModelViewer freetype)
target_link_directories(GLModelViewer PUBLIC ${CMAKE_BINARY_DIR}/external/glfw/src)
target_link_libraries(GLModelViewer glfw3)
#target_link_libraries(GLModelViewer freetype)
target_link_libraries(GLModelViewer Vulkan::Vulkan)

target_add_kaitai(GLModelViewer)

add_definitions("-DNOMINMAX")

target_add_shader(GLModelViewer sample/sample.vert)
target_add_shader(GLModelViewer sample/sample.frag)
target_add_shader(GLModelViewer sample_alt/sample.vert)
target_add_shader(GLModelViewer sample_alt/sample.frag)
target_add_shader(GLModelViewer blit/blit.vert)
target_add_shader(GLModelViewer blit/blit.frag)
target_add_shader(GLModelViewer grid/grid.vert)
target_add_shader(GLModelViewer grid/grid.frag)
target_add_shader(GLModelViewer geometry/geometry.vert)
target_add_shader(GLModelViewer geometry/geometry.frag)