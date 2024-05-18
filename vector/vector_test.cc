#include "vector.h"

#include "gtest/gtest.h"

inline constexpr int DEFAULT_VALUE = 0;
inline constexpr int TEST_VALUE = 123456;

struct Foo {
  int value_;
  Foo(int x = DEFAULT_VALUE) : value_(x) {}
};

std::size_t remaining_size(const sgi::vector<Foo>& vec) {
  return vec.capacity() - vec.size();
}

TEST(vector, basic) {
  sgi::vector<Foo> vec1;
  EXPECT_EQ(vec1.size(), 0);
  EXPECT_EQ(vec1.begin(), nullptr);
  EXPECT_EQ(vec1.end(), nullptr);
  EXPECT_TRUE(vec1.empty());
  EXPECT_EQ(vec1.capacity(), 0);

  sgi::vector<Foo> vec2(0);
  EXPECT_EQ(vec2.size(), 0);
  EXPECT_EQ(vec2.begin(), nullptr);
  EXPECT_EQ(vec2.end(), nullptr);
  EXPECT_TRUE(vec2.empty());
  EXPECT_EQ(vec2.capacity(), 0);

  sgi::vector<Foo> vec3(10);
  EXPECT_EQ(vec3.size(), 10);
  EXPECT_NE(vec3.begin(), nullptr);
  EXPECT_NE(vec3.end(), nullptr);
  EXPECT_FALSE(vec3.empty());
  EXPECT_EQ(vec3.capacity(), 20);
  for (size_t i = 0; i < vec3.size(); i++) {
    EXPECT_EQ(vec3[i].value_, DEFAULT_VALUE);
    vec3[i].value_ = i + 1;
    EXPECT_EQ(vec3[i].value_, i + 1);
  }

  sgi::vector<Foo> vec4(10, TEST_VALUE);
  EXPECT_NE(vec4.begin(), nullptr);
  EXPECT_NE(vec4.end(), nullptr);
  EXPECT_FALSE(vec4.empty());
  EXPECT_EQ(vec4.capacity(), 20);
  EXPECT_EQ(vec4.size(), 10);
  for (size_t i = 0; i < vec4.size(); i++) {
    EXPECT_EQ(vec4[i].value_, TEST_VALUE);
    vec4[i].value_ = i + 1;
    EXPECT_EQ(vec4[i].value_, i + 1);
  }
  EXPECT_EQ(vec4.front().value_, 1);
  EXPECT_EQ(vec4.back().value_, 10);
}

TEST(vector, push_pop_back) {
  sgi::vector<Foo> vec1;
  EXPECT_TRUE(vec1.empty());

  vec1.push_back({TEST_VALUE});
  EXPECT_EQ(vec1.size(), 1);
  EXPECT_EQ(vec1.capacity(), 1);
  EXPECT_EQ(remaining_size(vec1), 0);
  EXPECT_EQ(vec1.front().value_, TEST_VALUE);

  sgi::vector<Foo> vec2(10, TEST_VALUE);
  EXPECT_EQ(vec2.size(), 10);
  EXPECT_EQ(vec2.capacity(), 20);

  EXPECT_EQ(remaining_size(vec2), 10);
  for (std::size_t i = 0; i < 10; i++) {
    vec2.push_back({TEST_VALUE});
    EXPECT_EQ(vec2.capacity(), 20);
  }
  EXPECT_EQ(vec2.size(), 10 + 10);
  EXPECT_EQ(remaining_size(vec2), 0);

  // expansion
  vec2.push_back({TEST_VALUE});
  EXPECT_EQ(vec2.size(), 21);
  EXPECT_EQ(vec2.capacity(), 40);
  for (std::size_t i = 0; i < vec2.size(); i++) {
    EXPECT_EQ(vec2[i].value_, TEST_VALUE);
  }

  for (std::size_t i = 0; i < 21; i++) {
    vec2.pop_back();
  }
  EXPECT_EQ(vec2.size(), 0);
  EXPECT_EQ(vec2.capacity(), 40);
  EXPECT_TRUE(vec2.empty());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}