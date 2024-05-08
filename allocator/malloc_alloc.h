#ifndef ALLOCATOR_MALLOC_ALLOC_H_
#define ALLOCATOR_MALLOC_ALLOC_H_

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>

namespace sgi {
using size_t = std::size_t;

// first level memory allocator
class MallocAlloc {
  using Func = void (*)();

 public:
  static void* Allocate(size_t n) {
    void* ptr = malloc(n);
    if (ptr == NULL) {
      ptr = OomMalloc(n);
    }
    return ptr;
  }

  static void Deallocate(void* p, size_t n) { std::free(p); }

  static void* Reallocate(void* p, size_t new_sz) {
    void* ptr = realloc(p, new_sz);
    if (ptr == NULL) {
      ptr = OomRealloc(p, new_sz);
    }
    return ptr;
  }

  static Func SetNewHandler(Func func) {
    Func old_handler = oom_handler_;
    oom_handler_ = func;
    return (old_handler);
  }

 private:
  static void* OomMalloc(size_t n);
  static void* OomRealloc(void* p, size_t n);
  static void (*oom_handler_)();
};

void (*MallocAlloc::oom_handler_)() = nullptr;

void* MallocAlloc::OomMalloc(size_t n) {
  void* res = NULL;
  for (;;) {
    if (oom_handler_ == NULL) {
      throw std::bad_alloc();
      exit(1);
    }

    (*oom_handler_)();
    res = std::malloc(n);
    if (res != NULL) {
      return res;
    }
  }
}

void* MallocAlloc::OomRealloc(void* p, size_t n) {
  void* res = NULL;
  for (;;) {
    if (oom_handler_ == NULL) {
      throw std::bad_alloc();
      exit(1);
    }

    (*oom_handler_)();
    res = std::realloc(p, n);
    if (res != NULL) {
      return res;
    }
  }
}

}  // namespace sgi

#endif  // ALLOCATOR_MALLOC_ALLOC_H_