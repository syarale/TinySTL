#ifndef ALLOCATOR_CONSTRUCT_H_
#define ALLOCATOR_CONSTRUCT_H_

#include <type_traits>

template <typename T>
inline void _construct(T* pointer) {
  new (reinterpret_cast<char*>(pointer)) T();
}

// TODO(leisy): supports class T1's constructor with multiple parameters
template <typename T1, typename T2>
inline void _construct(T1* pointer, const T2& value) {
  new (reinterpret_cast<char*>(pointer)) T1(value);
}

template <typename T>
inline void _destroy(T* pointer) {
  pointer->~T();
}

template <typename ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last,
                         std::true_type) { /* TODO*/
}

template <typename ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last,
                         std::false_type) { /* TODO*/
}

template <typename ForwardIterator>
inline void _destroy(ForwardIterator first, ForwardIterator last) { /* TODO */
}

template <typename T>
inline void construct(T* pointer) {
  _construct(pointer);
}

template <typename T1, typename T2>
inline void construct(T1* pointer, const T2& value) {
  _construct(pointer, value);
}

template <typename T>
inline void destroy(T* pointer) {
  _destroy(pointer);
}

template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
  _destroy(first, last);
}

#endif  // ALLOCATOR_CONSTRUCT_H_