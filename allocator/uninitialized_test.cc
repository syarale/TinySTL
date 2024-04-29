#include "uninitialized.h"

#include <cstring>
#include <type_traits>
#include <vector>

#include "gtest/gtest.h"

constexpr int INIT_VALUE = 100;
constexpr int NUM = 10;

class Foo {
 public:
  Foo(int value = INIT_VALUE) : value_(value) {}
  ~Foo() = default;
  int value() { return value_; }

 private:
  int value_;
};

class Temp {
 public:
  Temp(int value = INIT_VALUE) : value_(value) {}
  Temp(const Temp& t) { value_ = t.value_; }
  ~Temp() = default;
  int value() { return value_; }

 private:
  int value_;
};

TEST(Uninitialized, uninitialized_copy) {
  std::vector<Foo> foo_vec(NUM, INIT_VALUE * 2);
  std::vector<Foo> foo_res(NUM);
  auto it1 =
      sgi::uninitialized_copy(foo_vec.begin(), foo_vec.end(), foo_res.begin());
  EXPECT_EQ(it1, foo_res.end());

  for (int i = 0; i < foo_res.size(); i++) {
    EXPECT_EQ(foo_res[i].value(), INIT_VALUE * 2);
  }

  std::vector<Temp> temp_vec(NUM, INIT_VALUE * 2);
  std::vector<Temp> temp_res(NUM);
  auto it2 = sgi::uninitialized_copy(temp_vec.begin(), temp_vec.end(),
                                     temp_res.begin());
  EXPECT_EQ(it2, temp_res.end());
  for (int i = 0; i < temp_res.size(); i++) {
    EXPECT_EQ(temp_res[i].value(), INIT_VALUE * 2);
  }
}

TEST(Uninitialized, uninitialized_copy_char) {
  const char* p1 = "123456789";
  char p2[] = "000000000";
  sgi::uninitialized_copy(p1, p1 + strlen(p1), p2);
  EXPECT_EQ(strcmp(p1, p2), 0);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}