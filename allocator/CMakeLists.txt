cmake_minimum_required(VERSION 3.1)
project(allocator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

add_executable(construct_test construct_test.cc)
target_link_libraries(construct_test GTest::GTest GTest::Main)