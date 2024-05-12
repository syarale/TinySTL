cmake_minimum_required(VERSION 3.1)
project(iterator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

find_package(GTest REQUIRED)

add_executable(iterator_test iterator_test.cc)
target_link_libraries(iterator_test GTest::GTest GTest::Main)