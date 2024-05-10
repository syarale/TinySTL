#ifndef ALLOCATOR_ALLOC_H_
#define ALLOCATOR_ALLOC_H_

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>

#include "default_alloc.h"
#include "malloc_alloc.h"

namespace sgi {

// In fact, __USE_MALLOC_ALLOC macro is never defined in SGI-STL
#ifdef __USE_MALLOC_ALLOC
using alloc = MallocAlloc;
#else
using alloc = DefaultAlloc;
#endif  //  __USE_MALLOC_ALLOC

template <typename T, typename Alloc = alloc>
class allocator {
 public:
  static T* allocate(size_t n) {
    return n <= 0 ? nullptr : static_cast<T*>(Alloc::Allocate(n * sizeof(T)));
  }

  static T* allocate(void) {
    return static_cast<T*>(Alloc::Allocate(sizeof(T)));
  }

  static void deallocate(T* p, size_t n) {
    if (n != 0) {
      Alloc::Deallocate(p, n * sizeof(T));
    }
  }

  static void deallocate(T* p) { Alloc::Deallocate(p, sizeof(T)); }
};

}  // namespace sgi
#endif  // ALLOCATOR_ALLOC_H_
