#include "vector.h"

#include "gtest/gtest.h"

inline constexpr int DEFAULT_VALUE = 0;

struct Foo {
  int value_;
  Foo(int x = DEFAULT_VALUE) : value_(x) {}
};

TEST(vector, basic) {
  sgi::vector<Foo> vec1;
  EXPECT_EQ(vec1.size(), 0);
  EXPECT_EQ(vec1.begin(), nullptr);
  EXPECT_EQ(vec1.end(), nullptr);
  EXPECT_EQ(vec1.empty(), true);
  EXPECT_EQ(vec1.capacity(), 0);

  sgi::vector<Foo> vec2(0);
  EXPECT_EQ(vec2.size(), 0);
  EXPECT_EQ(vec2.begin(), nullptr);
  EXPECT_EQ(vec2.end(), nullptr);
  EXPECT_EQ(vec2.empty(), true);
  EXPECT_EQ(vec2.capacity(), 0);

  sgi::vector<Foo> vec3(10);
  EXPECT_EQ(vec3.size(), 10);
  EXPECT_NE(vec3.begin(), nullptr);
  EXPECT_NE(vec3.end(), nullptr);
  EXPECT_EQ(vec3.empty(), false);
  EXPECT_EQ(vec3.capacity(), 20);
  for (size_t i = 0; i < vec3.size(); i++) {
    EXPECT_EQ(vec3[i].value_, DEFAULT_VALUE);
    vec3[i].value_ = i + 1;
    EXPECT_EQ(vec3[i].value_, i + 1);
  }

  sgi::vector<Foo> vec4(10, 123456);
  EXPECT_NE(vec4.begin(), nullptr);
  EXPECT_NE(vec4.end(), nullptr);
  EXPECT_EQ(vec4.empty(), false);
  EXPECT_EQ(vec4.capacity(), 20);
  EXPECT_EQ(vec4.size(), 10);
  for (size_t i = 0; i < vec4.size(); i++) {
    EXPECT_EQ(vec4[i].value_, 123456);
    vec4[i].value_ = i + 1;
    EXPECT_EQ(vec4[i].value_, i + 1);
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}