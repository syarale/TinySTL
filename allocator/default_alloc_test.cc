#include "default_alloc.h"

#include "gtest/gtest.h"

using alloc = sgi::DefaultAlloc;

#define SIKP_PRIVATE_MEMBER_CASE 0

#ifndef SIKP_PRIVATE_MEMBER_CASE
// In order to test private member functions, you need to temporarily
// annotate the corresponding private keyword and also annotate the
// SIKP_PRIVATE_MEMBER_CASE macro.

int Count(void* head) {
  alloc::obj* ptr = static_cast<alloc::obj*>(head);
  int count = 0;
  while (ptr != nullptr) {
    count++;
    ptr = ptr->next_free_obj;
  }
  return count;
}

TEST(DefaultAlloc, RoundUp) {
  // Before running this test case, RoundUp should be temporarily
  // modified to a public attribute
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
  EXPECT_EQ(alloc::FreeListsIndex(128), 15);
  EXPECT_EQ(alloc::FreeListsIndex(120), 14);
  EXPECT_EQ(alloc::FreeListsIndex(88), 10);
  EXPECT_EQ(alloc::FreeListsIndex(56), 6);
  EXPECT_EQ(alloc::FreeListsIndex(16), 1);
  EXPECT_EQ(alloc::FreeListsIndex(8), 0);
}

TEST(DefaultAlloc, AllocChunk) {
  int nobjs = 1;
  char* ptr1 = alloc::AllocChunk(8, nobjs);
  EXPECT_NE(ptr1, nullptr);
  EXPECT_EQ(nobjs, 1);
  EXPECT_EQ(alloc::end_free_ - alloc::start_free_, 8);

  EXPECT_EQ(alloc::extend_heap_size_, 16);

  nobjs = 2;
  char* ptr2 = alloc::AllocChunk(8, nobjs);
  EXPECT_NE(ptr2, nullptr);
  EXPECT_EQ(nobjs, 1);
  EXPECT_EQ(alloc::end_free_ - alloc::start_free_, 0);

  nobjs = 2;
  char* ptr3 = alloc::AllocChunk(16, nobjs);
  EXPECT_NE(ptr3, nullptr);
  EXPECT_EQ(nobjs, 2);
  EXPECT_EQ(alloc::end_free_ - alloc::start_free_, 40);
  EXPECT_EQ(alloc::extend_heap_size_, 88);

  nobjs = 2;
  char* ptr4 = alloc::AllocChunk(24, nobjs);
  EXPECT_NE(ptr4, nullptr);
  EXPECT_EQ(nobjs, 1);
  EXPECT_EQ(alloc::end_free_ - alloc::start_free_, 16);
  EXPECT_EQ(alloc::extend_heap_size_, 88);

  nobjs = 2;
  char* ptr5 = alloc::AllocChunk(24, nobjs);
  EXPECT_NE(ptr5, nullptr);
  EXPECT_EQ(nobjs, 2);
  EXPECT_EQ(alloc::end_free_ - alloc::start_free_, 56);

  int index = alloc::FreeListsIndex(16);
  EXPECT_NE(alloc::free_lists_[index], nullptr);
  EXPECT_EQ(alloc::free_lists_[index]->next_free_obj, nullptr);

  size_t bytes = 100000000000;
  nobjs = 2;
  EXPECT_THROW(alloc::AllocChunk(bytes, nobjs), std::bad_alloc);
}

TEST(DefaultAlloc, Refill) {
  int bytes = 16;
  char* ptr1 = alloc::Refill(bytes);
  EXPECT_NE(ptr1, nullptr);
  int index = alloc::FreeListsIndex(bytes);
  EXPECT_NE(alloc::free_lists_[index], nullptr);
  EXPECT_EQ(Count(alloc::free_lists_[index]), sgi::DEFAULT_CHUNKS - 1);

  char* ptr2 = alloc::Refill(2 * bytes);
  EXPECT_NE(ptr2, nullptr);
  EXPECT_EQ(Count(alloc::free_lists_[index]), sgi::DEFAULT_CHUNKS - 1);
}

#endif

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}