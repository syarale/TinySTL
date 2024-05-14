cmake_minimum_required(VERSION 3.1)
project(vector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

find_package(GTest REQUIRED)

add_executable(vector_test vector_test.cc)
target_link_libraries(vector_test GTest::GTest GTest::Main)