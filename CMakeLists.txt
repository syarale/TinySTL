cmake_minimum_required(VERSION 3.1)
project(TinySTL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(allocator)
add_subdirectory(iterator)
add_subdirectory(vector)
add_subdirectory(list)