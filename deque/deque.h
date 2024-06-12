#ifndef DEQUE_DEQUE_H_
#define DEQUE_DEQUE_H_
#include <cstddef>

namespace sgi {

inline std::size_t _deque_buf_size(std::size_t n, std::size_t sz) {
  return n != 0 ? n : (sz < 512 ? std::size_t(512 / sz) : std::size_t(1));
}

template <typename T, typename Ref, typename Ptr, std::size_t BuffSize>
struct _deque_iterator {
  typedef _deque_iterator<T, Ref, Ptr, BuffSize> iterator;
  typedef _deque_iterator<T, const T&, const T*, BuffSize> const_iterator;

  typedef T value;
  typedef Ptr pointer;
  typedef Ref reference;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef T** map_pointer;

  typedef _deque_iterator self;

  T* curr_;
  T* first_;
  T* last_;
  map_pointer node_;

  static std::size_t buffer_size() {
    return _deque_buf_size(BuffSize, sizeof(T));
  }

  void set_node(map_pointer new_node);
  reference operator*() const;
  pointer operator->() const;

  self& operator++();
  self operator++(int);
  self& operator--();
  self operator--(int);

  self& operator+=(difference_type n);
  self operator+(difference_type n);

  self& operator-=(difference_type n);
  self operator-(difference_type n);

  self& operator[](difference_type n) const;

  bool operator==(const self& it) const;
  bool operator!=(const self& it) const;
  bool operator<(const self& it) const;
};

}  // namespace sgi

#endif  // DEQUE_DEQUE_H_