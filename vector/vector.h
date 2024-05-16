#ifndef VECTOR_VECTOR_H_
#define VECTOR_VECTOR_H_

#include "alloc.h"
#include "construct.h"
#include "exception.h"

namespace sgi {

template <typename T, typename Alloc = sgi::alloc>
class vector {
 public:
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator = value_type*;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  explicit vector() = default;
  explicit vector(size_type n) : vector(n, T()){};
  explicit vector(size_type n, const T& value);
  ~vector();

  iterator begin() const { return start_; }
  iterator end() const { return finish_; }
  bool empty() const { return start_ == finish_; }

  size_type size() const { return static_cast<size_type>(finish_ - start_); }
  size_type capacity() const {
    return static_cast<size_type>(end_of_storage_ - start_);
  }

  reference operator[](size_type n) { return *(start_ + n); }

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

template <typename T, typename Alloc>
inline vector<T, Alloc>::vector(size_type n, const T& value) {
  if (n < 0) {
    throw sgi::invalid_alloc("argument is invalid");
  }

  if (n > 0) {
    size_type size = 2 * n;
    start_ = static_cast<iterator>(data_allocator::allocate(size));
    end_of_storage_ = start_ + size;
    for (size_type i = 0; i < n; i++) {
      sgi::construct(start_ + i, value);
    }
    finish_ = start_ + n;
  }
}

template <typename T, typename Alloc>
inline vector<T, Alloc>::~vector() {
  sgi::destroy(start_, finish_);
  data_allocator::deallocate(start_, end_of_storage_ - start_);
}

}  // namespace sgi

#endif  // VECTOR_VECTOR_H_