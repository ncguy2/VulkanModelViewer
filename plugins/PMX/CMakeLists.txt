cmake_minimum_required(VERSION 3.18)
project(PMXPlugin)

set(CMAKE_CXX_STANDARD 20)

set(SRCS lib/PMXPlugin.cpp lib/loader/PMXLoader.cpp)
set(HDRS include/PMXPlugin.h include/loader/PMXLoader.h)

file(GLOB GEN_SRCS gen/*.cpp gen/*.h )

message("GEN_SRCS: ${GEN_SRCS}")

add_library(PMXPlugin SHARED ${HDRS} ${SRCS} ${GEN_SRCS})
target_include_directories(PMXPlugin PRIVATE include)
target_include_directories(PMXPlugin PUBLIC ${PLUGIN_HEADER_ROOT})
target_include_directories(PMXPlugin PUBLIC ../../external/kaitai_struct_cpp_stl_runtime)

#target_add_parser(PMXPlugin gen_src/vector_types.ksy)
target_add_parser(PMXPlugin gen_src/mmd_pmx.ksy)