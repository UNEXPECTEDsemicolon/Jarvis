cmake_minimum_required(VERSION 3.10)

project(jarvis)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(FMT_HEADER_ONLY)
include_directories(fmt/include)
add_subdirectory(fmt)
add_subdirectory(src)
