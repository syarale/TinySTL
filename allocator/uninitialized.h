#ifndef ALLOCATOR_UNINITIALIZED_H_
#define ALLOCATOR_UNINITIALIZED_H_

#include <algorithm>
#include <cstring>
#include <iterator>

#include "construct.h"

namespace sgi {

template <typename InputIter, typename ForwardIter>
inline ForwardIter uninitialized_copy_aux(InputIter first, InputIter last,
                                          ForwardIter result, std::true_type) {
  // TODO(leisy): use sgi::copy instead of std::copy
  ForwardIter it = std::copy(first, last, result);
  return it;
}

template <typename InputIter, typename ForwardIter>
inline ForwardIter uninitialized_copy_aux(InputIter first, InputIter last,
                                          ForwardIter result, std::false_type) {
  for (auto it = first; it != last; it++) {
    sgi::construct(&*result, *first);
    result++;
  }

  return result;
}

template <typename InputIter, typename ForwardIter>
inline ForwardIter uninitialized_copy(InputIter first, InputIter last,
                                      ForwardIter result) {
  using type = typename std::iterator_traits<InputIter>::value_type;
  uninitialized_copy_aux(first, last, result,
                         std::is_trivially_copyable<type>());
}

inline char* uninitialized_copy(const char* first, const char* last,
                                char* result) {
  memmove(result, first, last - first);
  return result + (last - first);
}

#ifdef __STDC_ISO_10646__
inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last,
                                   wchar_t* result) {
  memmove(result, first, sizeof(wchar_t) * (last - first));
  return result + (last - first);
}
#endif  // __STDC_ISO_10646__

template <typename InputIter, typename Size, typename ForwardIter>
inline ForwardIter uninitialized_copy_n(InputIter first, Size count,
                                        ForwardIter result) { /*TODO*/
}

template <typename InputIter, typename T>
inline void uninitialized_fill(InputIter first, InputIter last,
                               const T& value) { /*TODO*/
}

template <typename InputIter, typename Size, typename T>
inline void uninitialized_fill_n(InputIter first, Size count,
                                 const T& value) { /*TODO*/
}

}  // namespace sgi

#endif  // ALLOCATOR_UNINITIALIZED_H_