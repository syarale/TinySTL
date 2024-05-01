#include "alloc.h"

#include "gtest/gtest.h"

constexpr int NUM = 10;

TEST(Alloc, allocator) {
  sgi::allocator<int, sgi::alloc> alloc;
  int* p = alloc.allocate(NUM);
  alloc.deallocate(p, NUM);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}