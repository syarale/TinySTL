#ifndef VECTOR_VECTOR_H_
#define VECTOR_VECTOR_H_

#include "alloc.h"
#include "construct.h"
#include "exception.h"
#include "uninitialized.h"

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
  ~vector() { destroy_all(); }

  iterator begin() const { return start_; }
  iterator end() const { return finish_; }
  bool empty() const { return start_ == finish_; }

  size_type size() const { return static_cast<size_type>(finish_ - start_); }
  size_type capacity() const {
    return static_cast<size_type>(end_of_storage_ - start_);
  }

  reference operator[](size_type n) { return *(start_ + n); }
  reference front() { return *start_; }
  reference back() { return *(finish_ - 1); }

  void push_back(const T& value);
  void pop_back();

  iterator insert(iterator position, const T& value);
  iterator insert(iterator position, size_type n, const T& value);
  iterator erase(iterator position) {}
  iterator erase(iterator first, iterator last) {}
  void resize(size_type n) {}
  void resize(size_type n, const T& value) {}

  void clear() {}

 private:
  using data_allocator = sgi::allocator<value_type, Alloc>;

  void destroy_all();
  iterator insert_aux(iterator position, const T& value);

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
inline void vector<T, Alloc>::push_back(const T& value) {
  if (finish_ == end_of_storage_) {
    insert_aux(end(), value);
  } else {
    *finish_ = value;
    ++finish_;
  }
}

// An error will result when pop_back is called on an empty vector.
// This is ensured by the user.
template <typename T, typename Alloc>
inline void vector<T, Alloc>::pop_back() {
  sgi::destroy(finish_ - 1);
  --finish_;
}

template <typename T, typename Alloc>
inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(
    iterator position, const T& value) {
  return insert_aux(position, value);
}

template <typename T, typename Alloc>
inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(
    iterator position, size_type n, const T& value) {
  size_type remaining_size = static_cast<size_type>(end_of_storage_ - finish_);

  // trigger expansion
  if (remaining_size < n) {
    size_type new_size = size() + std::max(size(), n);
    iterator new_start_ =
        static_cast<iterator>(data_allocator::allocate(new_size));

    iterator pos = sgi::uninitialized_copy(start_, position, new_start_);
    sgi::uninitialized_fill(pos, pos + n, value);
    finish_ = sgi::uninitialized_copy(position, finish_, pos + n);
    start_ = new_start_;
    end_of_storage_ = start_ + new_size;
    return pos;
  }

  // no expansion
  size_type move_size = static_cast<size_type>(finish_ - position);
  if (n >= move_size) {
    sgi::uninitialized_copy(position, finish_, position + n);
    uninitialized_fill(finish_, position + n, value);
    std::fill(position, finish_, value);  // TODO(leisy): use sgi::fill
  } else {
    size_type left_size = move_size - n;
    uninitialized_copy(position + left_size, finish_, finish_);
    std::copy_backward(position, position + left_size, finish_);
    std::fill(position, position + n, value);
  }
  finish_ += n;
  return position;
}

template <typename T, typename Alloc>
inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert_aux(
    iterator position, const T& value) {
  if (finish_ != end_of_storage_) {
    // TODO(leisy): use sgi::copy_backward
    std::copy_backward(position, finish_, finish_ + 1);
    ++finish_;
    *position = value;
    return position;
  }

  // expansion
  size_type new_size = (size() == 0) ? 1 : 2 * size();
  iterator new_start_ =
      static_cast<iterator>(data_allocator::allocate(new_size));
  iterator new_pos = sgi::uninitialized_copy(start_, position, new_start_);
  *new_pos = value;
  iterator new_finish = sgi::uninitialized_copy(position, finish_, new_pos + 1);

  destroy_all();
  start_ = new_start_;
  finish_ = new_finish;
  end_of_storage_ = start_ + new_size;

  return new_pos;
}

template <typename T, typename Alloc>
inline void vector<T, Alloc>::destroy_all() {
  sgi::destroy(start_, finish_);
  data_allocator::deallocate(start_, capacity());
}

}  // namespace sgi

#endif  // VECTOR_VECTOR_H_