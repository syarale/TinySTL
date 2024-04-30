#include "uninitialized.h"

#include <cstring>
#include <memory>
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
  std::vector<int> vec(NUM);
  for (int i = 0; i < vec.size(); i++) {
    vec[i] = i * INIT_VALUE;
  }

  std::allocator<Foo> foo_alloc;
  Foo* foo_res = foo_alloc.allocate(NUM);
  auto foo_res_it = sgi::uninitialized_copy(vec.begin(), vec.end(), foo_res);

  EXPECT_EQ(foo_res_it, foo_res + NUM);
  for (int i = 0; i < NUM; i++) {
    EXPECT_EQ((foo_res + i)->value(), i * INIT_VALUE);
  }

  std::allocator<Temp> temp_alloc;
  Temp* temp_res = temp_alloc.allocate(NUM);
  auto temp_res_it = sgi::uninitialized_copy(vec.begin(), vec.end(), temp_res);
  EXPECT_EQ(temp_res_it, temp_res + NUM);
  for (int i = 0; i < NUM; i++) {
    EXPECT_EQ((temp_res + i)->value(), i * INIT_VALUE);
  }

  foo_alloc.deallocate(foo_res, NUM);
  temp_alloc.deallocate(temp_res, NUM);
}

TEST(Uninitialized, uninitialized_copy_char) {
  const char* p1 = "123456789";
  char p2[] = "000000000";
  sgi::uninitialized_copy(p1, p1 + strlen(p1), p2);
  EXPECT_EQ(strcmp(p1, p2), 0);
}

TEST(Uninitialized, uninitialized_fill) {
  std::allocator<Foo> foo_alloc;
  Foo* foo_first = foo_alloc.allocate(NUM);
  sgi::uninitialized_fill(foo_first, foo_first + NUM, INIT_VALUE);
  for (int i = 0; i < NUM; i++) {
    EXPECT_EQ((foo_first + i)->value(), INIT_VALUE);
  }

  std::allocator<Temp> temp_alloc;
  Temp* temp_first = temp_alloc.allocate(NUM);
  sgi::uninitialized_fill(temp_first, temp_first + NUM, INIT_VALUE);
  for (int i = 0; i < NUM; i++) {
    EXPECT_EQ((temp_first + i)->value(), INIT_VALUE);
  }

  foo_alloc.deallocate(foo_first, NUM);
  temp_alloc.deallocate(temp_first, NUM);
}

TEST(Uninitialized, uninitialized_fill_n) {
  std::allocator<Foo> foo_alloc;
  Foo* foo_first = foo_alloc.allocate(NUM);
  auto foo_it = sgi::uninitialized_fill_n(foo_first, NUM, INIT_VALUE);
  EXPECT_EQ(foo_it, foo_first + NUM);
  for (int i = 0; i < NUM; i++) {
    EXPECT_EQ((foo_first + i)->value(), INIT_VALUE);
  }

  std::allocator<Temp> temp_alloc;
  Temp* temp_first = temp_alloc.allocate(NUM);
  auto temp_it = sgi::uninitialized_fill_n(temp_first, NUM, INIT_VALUE);
  EXPECT_EQ(temp_it, temp_first + NUM);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}