cmake_minimum_required(VERSION 3.1)
project(deque)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

find_package(GTest REQUIRED)

include_directories(../allocator)
include_directories(../iterator)

add_executable(deque_test deque_test.cc)
target_link_libraries(deque_test GTest::GTest GTest::Main)