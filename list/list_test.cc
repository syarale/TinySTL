#include "list.h"

#include <random>

#include "gtest/gtest.h"

inline constexpr int DEFAULT_VAL = 101;

struct Foo {
  int value_ = 0;
  Foo() = default;
  bool operator==(const Foo& foo) { return value_ == foo.value_; }
  Foo(int val) : value_(val) {}
};

int GetRandomInt(int min_val, int max_val) {
  // assert(min_val < max_val);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(min_val, max_val);
  return distrib(gen);
}

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
  EXPECT_EQ(foo_list.begin(), foo_list.end());
  EXPECT_FALSE(foo_list.begin() != foo_list.end());
}

TEST(list, push_pop) {
  sgi::list<Foo> foo_list;
  for (int i = 9; i >= 0; i--) {
    foo_list.push_front(Foo(i));
    EXPECT_EQ(foo_list.begin()->value_, i);

    Foo& foo = foo_list.front();
    EXPECT_EQ(foo.value_, i);
    foo.value_ *= 10;
  }
  EXPECT_EQ(foo_list.size(), 10);

  for (int i = 10; i < 20; i++) {
    foo_list.push_back(Foo(i));
    EXPECT_EQ((--foo_list.end())->value_, i);

    Foo& foo = foo_list.back();
    EXPECT_EQ(foo.value_, i);
    foo.value_ *= 10;
  }
  EXPECT_EQ(foo_list.size(), 20);

  int i = 0;
  for (auto it = foo_list.begin(); it != foo_list.end(); it++) {
    EXPECT_EQ(it->value_, i * 10);
    i++;
  }

  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(foo_list.begin()->value_, i * 10);
    EXPECT_EQ(foo_list.front().value_, i * 10);
    foo_list.pop_front();
  }
  EXPECT_EQ(foo_list.size(), 10);

  for (int i = 19; i >= 10; i--) {
    EXPECT_EQ((--foo_list.end())->value_, i * 10);
    EXPECT_EQ(foo_list.back().value_, i * 10);
    foo_list.pop_back();
  }
  EXPECT_TRUE(foo_list.empty());
  EXPECT_EQ(foo_list.size(), 0);
}

TEST(list, insert_erase) {
  sgi::list<Foo> foo_list;
  for (int i = 9; i >= 0; i--) {
    auto it = foo_list.insert(foo_list.begin(), Foo(i));
    EXPECT_EQ(it, foo_list.begin());
    EXPECT_EQ((*it).value_, i);
  }
  EXPECT_EQ(foo_list.size(), 10);

  for (int i = 20; i < 30; i++) {
    auto it = foo_list.insert(foo_list.end(), Foo(i));
    EXPECT_EQ(it, --foo_list.end());
    EXPECT_EQ((*it).value_, i);
  }
  EXPECT_EQ(foo_list.size(), 20);

  auto it = foo_list.begin();
  int count = 10;
  while (count-- > 0) {
    ++it;
  }

  for (int i = 19; i >= 10; i--) {
    it = foo_list.insert(it, Foo(i));
    EXPECT_EQ((*it).value_, i);
  }
  EXPECT_EQ(foo_list.size(), 30);

  count = 0;
  for (auto it = foo_list.begin(); it != foo_list.end(); it++) {
    EXPECT_EQ((*it).value_, count++);
  }

  it = foo_list.begin();
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(it, foo_list.begin());
    EXPECT_EQ((*it).value_, i);
    it = foo_list.erase(it);
  }
  EXPECT_EQ((*it).value_, 10);
  EXPECT_EQ(foo_list.size(), 20);

  ++it;
  for (int i = 11; i < 29; i++) {
    EXPECT_EQ((*it).value_, i);
    it = foo_list.erase(it);
  }
  EXPECT_EQ(foo_list.size(), 2);
  EXPECT_EQ(foo_list.front().value_, 10);
  EXPECT_EQ(foo_list.back().value_, 29);
}

TEST(list, remove) {
  sgi::list<Foo> foo_list;
  for (int i = 0; i < 20; i++) {
    auto it = foo_list.insert(foo_list.end(), Foo(i));
    if (i % 2 == 0) {
      foo_list.insert(it, Foo(DEFAULT_VAL));
    }
  }
  EXPECT_EQ(foo_list.size(), 30);

  foo_list.remove(Foo(DEFAULT_VAL));
  EXPECT_EQ(foo_list.size(), 20);

  int count = 0;
  for (auto it = foo_list.begin(); it != foo_list.end(); it++) {
    EXPECT_EQ((*it).value_, count++);
  }
  EXPECT_EQ(foo_list.size(), 20);
}

TEST(list, clear) {
  sgi::list<Foo> foo_list;
  for (int i = 0; i < 20; i++) {
    foo_list.insert(foo_list.end(), Foo(i));
  }
  EXPECT_EQ(foo_list.size(), 20);

  foo_list.clear();
  EXPECT_TRUE(foo_list.empty());
  EXPECT_EQ(foo_list.size(), 0);
}

TEST(list, unique) {
  sgi::list<Foo> foo_list;
  int count = 0;
  for (int i = 0; i < 20; i++) {
    foo_list.insert(foo_list.end(), Foo(i));
    if (i == 10 || i == 15) {
      int num = GetRandomInt(1, 10);
      for (int j = 0; j < num; j++) {
        foo_list.insert(foo_list.end(), Foo(i));
      }
      count += num;
    }
  }
  count += 20;
  EXPECT_EQ(foo_list.size(), count);

  foo_list.unique();
  EXPECT_EQ(foo_list.size(), 20);

  int value = 0;
  for (auto it = foo_list.begin(); it != foo_list.end(); it++) {
    EXPECT_EQ(it->value_, value++);
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}