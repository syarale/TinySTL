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
  ~list() {}

  iterator begin() const { return iterator(dummy_node_->next); }
  iterator end() const { return iterator(dummy_node_); }
  bool empty() const { return dummy_node_->next == dummy_node_; }

  // Warning: The time complexity of size is O(n), which is to keep the time
  // complexity of splice O(1), but the C++11 standard requires the time
  // complexity of size to be O(1)
  size_type size() const;

  reference front() { return *begin(); }   // empty list results in UB
  reference back() { return *(--end()); }  // empty list results in UB

  void push_front(const T& val);
  void push_back(const T& val);
  void pop_front();  // empty list results in UB
  void pop_back();   // empty list results in UB

  iterator insert(iterator position, const T& val);
  iterator erase(iterator position);
  void remove(const T& value);
  void unique();  // need to ensure that the list is sorted
  void clear();

  void splice(iterator position, list& lst);
  void splice(iterator position, list&, iterator it);
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
};

template <typename T, typename Alloc>
inline void list<T, Alloc>::init_empty_list() {
  dummy_node_ = allocate_node();
  dummy_node_->prev = dummy_node_;
  dummy_node_->next = dummy_node_;
}

template <typename T, typename Alloc>
inline void list<T, Alloc>::push_front(const T& val) {
  link_type node = create_node(val);
  link_type head = dummy_node_->next;
  node->next = head;
  node->prev = dummy_node_;
  head->prev = node;
  dummy_node_->next = node;
}

template <typename T, typename Alloc>
inline void list<T, Alloc>::push_back(const T& val) {
  link_type tail = dummy_node_->prev;
  link_type node = create_node(val);
  node->prev = tail;
  node->next = dummy_node_;
  tail->next = node;
  dummy_node_->prev = node;
}

template <typename T, typename Alloc>
inline void list<T, Alloc>::pop_front() {
  link_type head = dummy_node_->next;
  link_type new_head = head->next;
  dummy_node_->next = new_head;
  new_head->prev = dummy_node_;
  destroy_node(head);
}

template <typename T, typename Alloc>
inline void list<T, Alloc>::pop_back() {
  link_type tail = dummy_node_->prev;
  link_type new_tail = tail->prev;
  new_tail->next = dummy_node_;
  dummy_node_->prev = new_tail;
  destroy_node(tail);
}

template <typename T, typename Alloc>
inline typename list<T, Alloc>::iterator list<T, Alloc>::insert(
    iterator position, const T& val) {
  link_type node = create_node(val);
  node->next = position.node_;
  node->prev = position.node_->prev;
  (position.node_->prev)->next = node;
  position.node_->prev = node;
  return iterator(node);
}

template <typename T, typename Alloc>
inline typename list<T, Alloc>::iterator list<T, Alloc>::erase(
    iterator position) {
  link_type prev_node = position.node_->prev;
  link_type next_node = position.node_->next;
  prev_node->next = next_node;
  next_node->prev = prev_node;
  destroy_node(position.node_);
  return iterator(next_node);
}

template <typename T, typename Alloc>
inline void list<T, Alloc>::remove(const T& value) {
  auto it = begin();
  while (it != end()) {
    if ((*it) == value) {
      it = erase(it);
    } else {
      ++it;
    }
  }
}

template <typename T, typename Alloc>
inline void list<T, Alloc>::clear() {
  auto it = begin();
  while (it != end()) {
    it = erase(it);
  }
}

template <typename T, typename Alloc>
inline void list<T, Alloc>::unique() {
  if (empty() || size() == 1) {
    return;
  }

  auto prev_it = begin();
  auto it = ++begin();
  while (it != end()) {
    if (*it == *prev_it) {
      it = erase(it);
    } else {
      ++prev_it;
      ++it;
    }
  }
}

template <typename T, typename Alloc>
inline typename list<T, Alloc>::size_type list<T, Alloc>::size() const {
  auto it = begin();
  size_type count = 0;
  while (it++ != end()) {
    ++count;
  }
  return count;
}

template <typename T, typename Alloc>
inline void list<T, Alloc>::transfer(iterator position, iterator first,
                                     iterator last) {
  if (first == last) {
    return;
  }

  link_type prev_node = first.node_->prev;
  --last;
  link_type next_node = last.node_->next;
  prev_node->next = next_node;
  next_node->prev = prev_node;

  link_type prev_pos_node = position.node_->prev;
  prev_pos_node->next = first.node_;
  first.node_->prev = prev_pos_node;
  position.node_->prev = last.node_;
  last.node_->next = position.node_;
}

template <typename T, typename Alloc>
inline void list<T, Alloc>::splice(iterator position, list& lst) {
  if (!lst.empty()) {
    transfer(position, lst.begin(), lst.end());
  }
}

template <typename T, typename Alloc>
inline void list<T, Alloc>::splice(iterator position, list&, iterator it) {
  auto next_it = it;
  ++next_it;
  if (it != next_it) {
    transfer(position, it, next_it);
  }
}

template <typename T, typename Alloc>
inline void list<T, Alloc>::splice(iterator position, list&, iterator first,
                                   iterator last) {
  if (first != last) {
    transfer(position, first, last);
  }
}

}  // namespace sgi
#endif  // LIST_LIST_H_