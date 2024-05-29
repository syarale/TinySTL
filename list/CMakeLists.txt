cmake_minimum_required(VERSION 3.1)
project(list)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

find_package(GTest REQUIRED)

include_directories(../allocator)
include_directories(../iterator)

add_executable(list_test list_test.cc)
target_link_libraries(list_test GTest::GTest GTest::Main)