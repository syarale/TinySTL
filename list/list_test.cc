#include "list.h"

#include "gtest/gtest.h"

struct Foo {
  int value_ = 0;
};

TEST(list, basic_apis) {
  sgi::list<Foo> foo_list;
  EXPECT_TRUE(foo_list.empty());
  EXPECT_EQ(foo_list.size(), 0);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}