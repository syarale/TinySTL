#include "default_alloc.h"

#include "gtest/gtest.h"

#define SIKP_PRIVATE_MEMBER_CASE 0

#ifndef SIKP_PRIVATE_MEMBER_CASE
TEST(DefaultAlloc, RoundUp) {
  // Before running this test case, RoundUp should be temporarily
  // modified to a public attribute

  using alloc = sgi::default_alloc;
  EXPECT_EQ(alloc::RoundUp(128), 128);
  EXPECT_EQ(alloc::RoundUp(110), 112);
  EXPECT_EQ(alloc::RoundUp(90), 96);
  EXPECT_EQ(alloc::RoundUp(88), 88);
  EXPECT_EQ(alloc::RoundUp(56), 56);
  EXPECT_EQ(alloc::RoundUp(2), 8);
  EXPECT_EQ(alloc::RoundUp(0), 0);
}

TEST(DefaultAlloc, FreeListsIndex) {
  // Before running this test case, FreeListsIndex should be temporarily
  // modified to a public attribute
  using alloc = sgi::default_alloc;
  EXPECT_EQ(alloc::FreeListsIndex(128), 15);
  EXPECT_EQ(alloc::FreeListsIndex(120), 14);
  EXPECT_EQ(alloc::FreeListsIndex(88), 10);
  EXPECT_EQ(alloc::FreeListsIndex(56), 6);
  EXPECT_EQ(alloc::FreeListsIndex(16), 1);
  EXPECT_EQ(alloc::FreeListsIndex(8), 0);
}
#endif

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}