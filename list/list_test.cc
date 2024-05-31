#include "list.h"

#include "gtest/gtest.h"

inline constexpr int DEFAULT_VAL = 101;

struct Foo {
  int value_ = 0;
  Foo() = default;
  Foo(int val) : value_(val) {}
};

TEST(list_iterator, basic_apis) {
  using list_iterator = sgi::list_iterator<Foo, Foo&, Foo*>;
  sgi::_list_node<Foo>* ptr = new sgi::_list_node<Foo>();
  ptr->prev = ptr;
  ptr->next = ptr;
  ptr->data = Foo(DEFAULT_VAL);

  list_iterator it1;
  list_iterator it2(ptr);
  list_iterator it3(it2);

  EXPECT_EQ(it1.node_, nullptr);
  EXPECT_EQ(it2.node_, ptr);
  EXPECT_EQ(it3, it2);
  EXPECT_FALSE(it2 != it3);

  EXPECT_EQ((*it2).value_, DEFAULT_VAL);
  EXPECT_EQ(it3->value_, DEFAULT_VAL);
  EXPECT_EQ(it2++, it2);
  EXPECT_EQ(++it2, it2);
  EXPECT_EQ(it3--, it3);
  EXPECT_EQ(--it3, it3);

  delete ptr;
}

TEST(list, basic_apis) {
  sgi::list<Foo> foo_list;
  EXPECT_TRUE(foo_list.empty());
  EXPECT_EQ(foo_list.size(), 0);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}