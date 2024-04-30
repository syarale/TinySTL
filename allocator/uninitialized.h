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
  auto result_bk = result;
  try {
    for (auto it = first; it != last; it++) {
      sgi::construct(&*result, *it);
      result++;
    }
  } catch (...) {
    sgi::destroy(result_bk, result);  // commit or rollback
    result = result_bk;
  }
  return result;
}

template <typename InputIter, typename ForwardIter>
inline ForwardIter uninitialized_copy(InputIter first, InputIter last,
                                      ForwardIter result) {
  using type = typename std::iterator_traits<ForwardIter>::value_type;
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

template <typename ForwardIter, typename T>
inline void uninitialized_fill_aux(ForwardIter first, ForwardIter last,
                                   const T& value, std::true_type) {
  // TODO(leisy): use sgi::fill instead of std::file
  std::fill(first, last, value);
}

template <typename ForwardIter, typename T>
inline void uninitialized_fill_aux(ForwardIter first, ForwardIter last,
                                   const T& value, std::false_type) {
  auto first_bk = first;
  try {
    for (; first != last; first++) {
      sgi::construct(&*first, value);
    }
  } catch (...) {
    sgi::destroy(first_bk, first);  // commit or rollback
  }
}

template <typename ForwardIter, typename T>
inline void uninitialized_fill(ForwardIter first, ForwardIter last,
                               const T& value) {
  using type = typename std::iterator_traits<ForwardIter>::value_type;
  uninitialized_fill_aux(first, last, value,
                         std::is_trivially_copyable<type>());
}

template <typename ForwardIter, typename Size, typename T>
inline ForwardIter uninitialized_fill_n_aux(ForwardIter first, Size count,
                                            const T& value, std::true_type) {
  // TODO(leisy): use sgi::fill_n instead of std::fill_n
  return std::fill_n(first, count, value);
}

template <typename ForwardIter, typename Size, typename T>
inline ForwardIter uninitialized_fill_n_aux(ForwardIter first, Size count,
                                            const T& value, std::false_type) {
  auto first_bk = first;
  try {
    for (; count > 0; count--) {
      sgi::construct(&*first, value);
      first++;
    }
  } catch (...) {
    sgi::destroy(first_bk, first);  // commit or rollback
    first = first_bk;
  }
  return first;
}

template <typename ForwardIter, typename Size, typename T>
inline ForwardIter uninitialized_fill_n(ForwardIter first, Size count,
                                        const T& value) {
  using type = typename std::iterator_traits<ForwardIter>::value_type;
  return uninitialized_fill_n_aux(first, count, value,
                                  std::is_trivially_copyable<type>());
}

}  // namespace sgi

#endif  // ALLOCATOR_UNINITIALIZED_H_