#ifndef LIST_LIST_H_
#define LIST_LIST_H_

#include "alloc.h"
#include "construct.h"
#include "iterator.h"

namespace sgi {

template <typename T>
struct _list_node {
  _list_node<T>* prev;
  _list_node<T>* next;
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
  using link_type = _list_node<T>*;

  link_type node_;

  list_iterator(link_type ptr = nullptr) : node_(ptr) {}
  list_iterator(const iterator& it) : node_(it.node_) {}

  bool operator==(const self& it) const { return node_ == it.node_; }
  bool operator!=(const self& it) const { return node_ != it.node_; }

  reference operator*() const { return node_->data; }
  pointer operator->() const { return &(node_->data); }

  self& operator++() {
    node_ = node_->next;
    return *this;
  }

  self operator++(int) {
    list_iterator old_it = *this;
    node_ = node_->next;
    return old_it;
  }

  self& operator--() {
    node_ = node_->prev;
    return *this;
  }

  self operator--(int) {
    list_iterator old_it = *this;
    node_ = node_->prev;
    return old_it;
  }
};

template <typename T, typename Alloc = alloc>
class list {
  using iterator = list_iterator<T, T&, T*>;
  using list_node = _list_node<T>;
  using link_type = _list_node<T>*;
  using size_type = std::size_t;
  using reference = T&;

 public:
  list() { init_empty_list(); }

  iterator begin() { return iterator(dummy_node_->next); }
  iterator end() { return iterator(dummy_node_); }

  bool empty() const { return dummy_node_->next == dummy_node_; }
  size_type size() const { return count_; }

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

  //  private:
  using node_allocator = sgi::allocator<list_node, Alloc>;

  link_type allocate_node() {
    return static_cast<link_type>(node_allocator::allocate());
  }

  void deallocate_node(link_type p) { node_allocator::deallocate(p); }

  link_type create_node(const T& val) {
    link_type node = allocate_node();
    sgi::construct(&(node->data), val);
    return node;
  }

  void destroy_node(link_type node) {
    sgi::destroy(&(node->data));
    deallocate_node(node);
  }

  void init_empty_list();
  void transfer(iterator position, iterator first, iterator last);

  link_type dummy_node_;
  size_type count_ = 0;
};

template <typename T, typename Alloc>
inline void list<T, Alloc>::init_empty_list() {
  dummy_node_ = allocate_node();
  dummy_node_->prev = dummy_node_;
  dummy_node_->next = dummy_node_;
}

}  // namespace sgi
#endif  // LIST_LIST_H_