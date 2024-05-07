#include "alloc.h"

#include "construct.h"
#include "gtest/gtest.h"

#define SIKP_PRIVATE_MEMBER_CASE 0

constexpr int NUM = 10;
constexpr int INIT_VALUE = 101;

class Foo {
 public:
  int value = INIT_VALUE;
};

TEST(MallocAlloc, malloc_alloc) {
  sgi::allocator<Foo, sgi::malloc_alloc> alloc;
  Foo* p = alloc.allocate(NUM);
  EXPECT_NE(p, nullptr);
  for (int i = 0; i < NUM; i++) {
    sgi::construct(p + i);
    EXPECT_EQ((p + i)->value, INIT_VALUE);
  }

  sgi::destroy(p, p + NUM);
  alloc.deallocate(p, NUM);
}

#ifndef SIKP_PRIVATE_MEMBER_CASE
TEST(DefaultAlloc, round_up) {
  // Before running this test case, round_up should be temporarily
  // modified to a public attribute

  using alloc = sgi::default_alloc;
  EXPECT_EQ(alloc::round_up(128), 128);
  EXPECT_EQ(alloc::round_up(110), 112);
  EXPECT_EQ(alloc::round_up(90), 96);
  EXPECT_EQ(alloc::round_up(88), 88);
  EXPECT_EQ(alloc::round_up(56), 56);
  EXPECT_EQ(alloc::round_up(2), 8);
  EXPECT_EQ(alloc::round_up(0), 0);
}

TEST(DefaultAlloc, free_lists_index) {
  // Before running this test case, free_lists_index should be temporarily
  // modified to a public attribute
  using alloc = sgi::default_alloc;
  EXPECT_EQ(alloc::free_lists_index(128), 15);
  EXPECT_EQ(alloc::free_lists_index(120), 14);
  EXPECT_EQ(alloc::free_lists_index(88), 10);
  EXPECT_EQ(alloc::free_lists_index(56), 6);
  EXPECT_EQ(alloc::free_lists_index(16), 1);
  EXPECT_EQ(alloc::free_lists_index(8), 0);
}
#endif

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}