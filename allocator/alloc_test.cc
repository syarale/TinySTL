#include "alloc.h"

#include "gtest/gtest.h"

TEST(allocator, allocator) {
  sgi::allocator<int> alloc;
  int* ptr1 = alloc.allocate(100);
  int* ptr2 = alloc.allocate();
  alloc.deallocate(ptr1, 100);
  alloc.deallocate(ptr2);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}