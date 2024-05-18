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
  Foo value(TEST_VALUE);
  EXPECT_TRUE(vec1.empty());

  vec1.push_back(value);
  EXPECT_EQ(vec1.size(), 1);
  EXPECT_EQ(vec1.capacity(), 1);
  EXPECT_EQ(remaining_size(vec1), 0);
  EXPECT_EQ(vec1.front().value_, TEST_VALUE);

  sgi::vector<Foo> vec2(10, TEST_VALUE);
  EXPECT_EQ(vec2.size(), 10);
  EXPECT_EQ(vec2.capacity(), 20);

  EXPECT_EQ(remaining_size(vec2), 10);
  for (std::size_t i = 0; i < 10; i++) {
    vec2.push_back(value);
    EXPECT_EQ(vec2.capacity(), 20);
  }
  EXPECT_EQ(vec2.size(), 10 + 10);
  EXPECT_EQ(remaining_size(vec2), 0);

  // expansion
  vec2.push_back(value);
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

TEST(vector, insert) {
  sgi::vector<Foo> vec;
  Foo value(TEST_VALUE);

  EXPECT_EQ(vec.size(), 0);
  vec.insert(vec.end(), value);  // expansion
  EXPECT_EQ(vec.size(), 1);
  EXPECT_EQ(remaining_size(vec), 0);
  EXPECT_EQ(vec.front().value_, TEST_VALUE);

  auto it = vec.insert(vec.begin(), value);
  EXPECT_EQ(vec.size(), 2);
  EXPECT_EQ(remaining_size(vec), 0);
  EXPECT_EQ(it->value_, TEST_VALUE);

  auto it1 = vec.insert(it, value);  // expansion
  EXPECT_EQ(vec.capacity(), 4);
  EXPECT_EQ(remaining_size(vec), 1);

  auto it2 = vec.insert(it1, value);
  EXPECT_EQ(vec.capacity(), 4);
  EXPECT_EQ(remaining_size(vec), 0);

  auto it3 = vec.insert(vec.begin(), value);  // expansion
  auto it4 = vec.insert(vec.end(), value);
  EXPECT_EQ(vec.size(), 6);
  EXPECT_EQ(vec.capacity(), 8);
  EXPECT_EQ(remaining_size(vec), 2);
  EXPECT_EQ(it4 + 1, vec.end());
  for (auto it = vec.begin(); it != vec.end(); it++) {
    EXPECT_EQ(it->value_, TEST_VALUE);
  }
}

TEST(vector, insert_range) {
  sgi::vector<Foo> vec;
  Foo value(TEST_VALUE);

  auto it = vec.insert(vec.begin(), 10, value);
  EXPECT_EQ(it, vec.begin());
  EXPECT_EQ(vec.size(), 10);
  EXPECT_EQ(vec.capacity(), 10);

  auto it1 = vec.insert(vec.end(), 10, value);
  EXPECT_EQ(it1, vec.end() - 10);
  EXPECT_EQ(vec.size(), 20);
  EXPECT_EQ(vec.capacity(), 20);

  auto it2 = vec.insert(vec.begin() + 10, 10, value);
  EXPECT_EQ(it2, vec.begin() + 10);
  EXPECT_EQ(vec.size(), 30);
  EXPECT_EQ(vec.capacity(), 40);

  auto it3 = vec.insert(vec.begin() + 1, 20, value);
  EXPECT_EQ(it3, vec.begin() + 1);
  EXPECT_EQ(vec.size(), 50);
  EXPECT_EQ(vec.capacity(), 60);

  auto it4 = vec.insert(vec.end() - 2, 5, value);
  EXPECT_EQ(it4, vec.end() - 7);
  EXPECT_EQ(vec.size(), 55);
  EXPECT_EQ(vec.capacity(), 60);

  auto it5 = vec.insert(vec.end() - 10, 5, value);
  EXPECT_EQ(it5, vec.end() - 15);
  EXPECT_EQ(vec.size(), 60);
  EXPECT_EQ(vec.capacity(), 60);
  for (auto it = vec.begin(); it != vec.end(); it++) {
    EXPECT_EQ(it->value_, TEST_VALUE);
  }
}

TEST(vector, clear) {
  sgi::vector<Foo> vec(10, {TEST_VALUE});
  EXPECT_EQ(vec.size(), 10);
  vec.clear();
  EXPECT_TRUE(vec.empty());
  EXPECT_EQ(vec.capacity(), 20);
}

TEST(vector, erase) {
  sgi::vector<Foo> vec(10, {TEST_VALUE});
  auto it1 = vec.erase(vec.end() - 1);
  EXPECT_EQ(it1, vec.end());
  EXPECT_EQ(vec.size(), 9);

  auto it2 = vec.erase(vec.begin(), vec.begin() + 2);
  EXPECT_EQ(it2, vec.begin());
  EXPECT_EQ(vec.size(), 7);

  auto it3 = vec.erase(vec.begin() + 2);
  EXPECT_EQ(it3, vec.begin() + 2);
  EXPECT_EQ(vec.size(), 6);

  auto it4 = vec.erase(vec.begin() + 1, vec.begin() + 4);
  EXPECT_EQ(it4, vec.begin() + 1);
  EXPECT_EQ(vec.size(), 3);
  for (auto it = vec.begin(); it != vec.end(); it++) {
    EXPECT_EQ(it->value_, TEST_VALUE);
  }

  auto it5 = vec.erase(vec.begin(), vec.end());
  EXPECT_EQ(it5, vec.end());
  EXPECT_TRUE(vec.empty());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}