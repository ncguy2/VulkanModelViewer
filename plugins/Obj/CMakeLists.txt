cmake_minimum_required(VERSION 3.18)
project(ObjPlugin)

set(CMAKE_CXX_STANDARD 20)

set(ObjPlugin_HDRS include/ObjPlugin.h include/ObjLoader.h include/tinyobjloader.h)
set(ObjPlugin_SRCS lib/ObjPlugin.cpp lib/ObjLoader.cpp lib/tinyobjloader.cpp)

if(EXISTS "gen")
    file(GLOB GEN_SRCS gen/*.cpp gen/*.h )
else()
    SET(GEN_SRCS )
endif()

add_library(ObjPlugin SHARED ${ObjPlugin_HDRS} ${ObjPlugin_SRCS} ${GEN_SRCS})
target_include_directories(ObjPlugin PRIVATE include)
if(DEFINED PLUGIN_HEADER_ROOT)
    target_include_directories(ObjPlugin PUBLIC ${PLUGIN_HEADER_ROOT})
else()
  target_include_directories(ObjPlugin PUBLIC api)
endif()
target_include_directories(ObjPlugin PUBLIC external/glm)
