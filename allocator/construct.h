#ifndef ALLOCATOR_CONSTRUCT_H_
#define ALLOCATOR_CONSTRUCT_H_

#include <iterator>
#include <type_traits>

template <typename T>
inline void _construct(T* pointer) {
  new (static_cast<void*>(pointer)) T();
}

// TODO(leisy): supports class T1's constructor with multiple parameters
template <typename T1, typename T2>
inline void _construct(T1* pointer, const T2& value) {
  new (static_cast<void*>(pointer)) T1(value);
}

template <typename T>
inline void _destroy(T* pointer) {
  pointer->~T();
}

template <typename ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last,
                         std::true_type) {
  /*
   * If object has trival destructor, there is nothing to do.
   */
}

template <typename ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last,
                         std::false_type) {
  for (auto it = first; it != last; it++) {
    _destroy(&*it);
  }
}

template <typename ForwardIterator>
inline void _destroy(ForwardIterator first, ForwardIterator last) {
  using type = typename std::iterator_traits<ForwardIterator>::value_type;
  _destroy_aux(first, last, typename std::is_trivially_destructible<type>());
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

inline void destroy(int*, int*) {}
inline void destroy(long*, long*) {}
inline void destroy(float*, float*) {}
inline void destroy(double*, double*) {}
inline void destroy(char*, char*) {}

#ifdef __STDC_ISO_10646__
inline void destroy(wchar_t*, wchar_t*) {}
#endif  // __STDC_ISO_10646__

#endif  // ALLOCATOR_CONSTRUCT_H_