#ifndef LIST_LIST_H_
#define LIST_LIST_H_

#include "alloc.h"
#include "iterator.h"

namespace sgi {

template <typename T>
struct list_node {
  list_node<T>* prev;
  list_node<T>* next;
  T data;
};

template <typename T, typename Ref, typename Ptr>
struct list_iterator {
  using value = T;
  using pointer = Ptr;
  using reference = Ref;
  using iterator_category = bidirectional_iterator_tag;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using iterator = list_iterator<T, T&, T*>;
  using self = list_iterator<T, Ref, Ptr>;
  using link_type = list_node<T>*;

  link_type node_;

  list_iterator(){};
  list_iterator(link_type x) : node_(x) {}
  list_iterator(const iterator& x) : node_(x.node_) {}

  bool operator==(const self& x) const;
  bool operator!=(const self& x) const;

  reference operator*() const;
  pointer operator->() const;

  self& operator++();
  self operator++(int);

  self& operator--();
  self operator--(int);
};

template <typename T, typename Alloc = alloc>
class list {
  using iterator = list_iterator<T, T&, T*>;
  using ListNode = list_node<T>;
  using LinkType = list_node<T>*;
  using size_type = std::size_t;
  using reference = T&;

 public:
  list();

  iterator begin();
  iterator end();

  bool empty() const;
  size_type size() const;

  reference front();
  reference back();

  void push_front(const T& x);
  void push_back(const T& x);
  void pop_front();
  void pop_back();

  iterator insert(iterator position, const T& x);
  iterator erase(iterator position);
  void remove(const T& value);
  void unique();
  void clear();

  void splice(iterator position, list& x);
  void splice(iterator position, list&, iterator i);
  void splice(iterator position, list&, iterator first, iterator last);

  void merge(list& x);
  void reverse();
  void sort();

 private:
  using list_node_allocator = sgi::allocator<ListNode, Alloc>;

  LinkType get_node();
  LinkType put_node(LinkType p);
  LinkType create_node(const T& x);
  void destroy_node(LinkType p);

  void empty_initialize();
  void transfer(iterator position, iterator first, iterator last);

  LinkType dummy_node_;
};

}  // namespace sgi
#endif  // LIST_LIST_H_