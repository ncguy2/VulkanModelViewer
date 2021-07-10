cmake_minimum_required(VERSION 3.18)
project(StdTexPlugin)

set(CMAKE_CXX_STANDARD 20)

set(StdTexPlugin_HDRS include/StdTexPlugin.h include/StdTexLoader.h include/stb_image.h)
set(StdTexPlugin_SRCS lib/StdTexPlugin.cpp lib/StdTexLoader.cpp lib/stb_image.cpp)

file(GLOB GEN_SRCS gen/*.cpp gen/*.h )

message("GEN_SRCS: ${GEN_SRCS}")

add_library(StdTexPlugin SHARED ${StdTexPlugin_HDRS} ${StdTexPlugin_SRCS} ${GEN_SRCS})
target_include_directories(StdTexPlugin PRIVATE include)
target_include_directories(StdTexPlugin PUBLIC ${PLUGIN_HEADER_ROOT})
target_include_directories(StdTexPlugin PUBLIC ../../external/kaitai_struct_cpp_stl_runtime)

target_add_parser(StdTexPlugin gen_src/gif.ksy)

add_definitions("-DNOMINMAX")