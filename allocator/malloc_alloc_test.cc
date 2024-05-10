#include "malloc_alloc.h"

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
  sgi::allocator<Foo, sgi::MallocAlloc> alloc;
  Foo* p = alloc.allocate(NUM);
  EXPECT_NE(p, nullptr);
  for (int i = 0; i < NUM; i++) {
    sgi::construct(p + i);
    EXPECT_EQ((p + i)->value, INIT_VALUE);
  }

  sgi::destroy(p, p + NUM);
  alloc.deallocate(p, NUM);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}