#include "construct.h"

#include "alloc.h"
#include "gtest/gtest.h"

constexpr int INIT_VALUE = 10;

class Foo {
 public:
  Foo(int value = INIT_VALUE) : value_(value) {}
  ~Foo() = default;
  int value() { return value_; }

  static int count;

 private:
  int value_;
};
int Foo::count = 100;

class Temp {
 public:
  Temp(int value = INIT_VALUE) : value_(value) {}
  ~Temp() { count--; }
  int value() { return value_; }

  static int count;

 private:
  int value_;
};
int Temp::count = 100;

TEST(Construct, construct) {
  sgi::allocator<Foo> alloc;
  Foo* ptr = alloc.allocate(2);

  sgi::construct(ptr);
  sgi::construct(ptr + 1, INIT_VALUE * 2);

  EXPECT_EQ(ptr->value(), INIT_VALUE);
  EXPECT_EQ((ptr + 1)->value(), INIT_VALUE * 2);
  alloc.deallocate(ptr, 2);
}

TEST(Construct, destroy) {
  sgi::allocator<Foo> foo_alloc;
  Foo* foo_ptr = foo_alloc.allocate(10);
  Foo* foo = foo_ptr;
  for (int i = 0; i < 10; i++) {
    sgi::construct(foo, 1010);
    EXPECT_EQ(foo->value(), 1010);
    foo++;
  }
  sgi::destroy(foo_ptr, foo_ptr + 9);
  EXPECT_EQ(Foo::count, 100);

  sgi::allocator<Temp> temp_alloc;
  Temp* temp_ptr = temp_alloc.allocate(10);
  Temp* temp = temp_ptr;
  for (int i = 0; i < 10; i++) {
    sgi::construct(temp, 1111);
    EXPECT_EQ(temp->value(), 1111);
    temp++;
  }
  sgi::destroy(temp_ptr, temp_ptr + 9);
  EXPECT_EQ(Temp::count, 100 - 9);

  foo_alloc.deallocate(foo_ptr, 10);
  temp_alloc.deallocate(temp_ptr, 10);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}