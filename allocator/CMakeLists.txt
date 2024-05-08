cmake_minimum_required(VERSION 3.1)
project(allocator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

find_package(GTest REQUIRED)

add_executable(construct_test construct_test.cc)
target_link_libraries(construct_test GTest::GTest GTest::Main)

add_executable(uninitialized_test uninitialized_test.cc)
target_link_libraries(uninitialized_test GTest::GTest GTest::Main)

add_executable(alloc_test alloc_test.cc)
target_link_libraries(alloc_test GTest::GTest GTest::Main)

add_executable(malloc_alloc_test malloc_alloc_test.cc)
target_link_libraries(malloc_alloc_test GTest::GTest GTest::Main)

add_executable(default_alloc_test default_alloc_test.cc)
target_link_libraries(default_alloc_test GTest::GTest GTest::Main)