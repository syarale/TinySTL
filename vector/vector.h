#ifndef VECTOR_VECTOR_H_
#define VECTOR_VECTOR_H_

#include "alloc.h"
#include "exception.h"

template <typename T, typename Alloc = sgi::alloc>
class vector {
 public:
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator = value_type*;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  vector() = default;
  vector(size_type n, const T& value) {}
  explicit vector(size_type n) {
    if (n < 0) {
      throw sgi::invalid_alloc("argument is invalid");
    }

    if (n == 0) {
      /*TODO(leisy)*/
    } else {
      /*TODO(leisy)*/
    }
  }

  ~vector() = default;

  void begin() {}
  void end() {}
  void size() {}
  void capacity() {}
  void empty() {}
  reference operator[](size_type n) {}

  reference front() {}
  reference back() {}
  void push_back() {}
  void pop_back() {}

  void insert(iterator position, const T& value) {}
  void insert(iterator position, size_type n, const T& value) {}
  iterator erase(iterator position) {}
  iterator erase(iterator first, iterator last) {}
  void resize(size_type n) {}
  void resize(size_type n, const T& value) {}

  void clear() {}

 private:
  using data_allocator = sgi::allocator<value_type, Alloc>;
  iterator start_ = nullptr;
  iterator finish_ = nullptr;
  iterator end_of_storage_ = nullptr;
};

#endif  // VECTOR_VECTOR_H_