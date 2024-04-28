#include "construct.h"

#include <string>

#include "gtest/gtest.h"

int add(int a, int b) { return a + b; }

TEST(fun, add_a) {
  EXPECT_EQ(-3, add(-2, -1));
  EXPECT_EQ(-2, add(1, -3));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}