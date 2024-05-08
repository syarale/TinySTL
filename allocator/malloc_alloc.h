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
class malloc_alloc {
  using Func = void (*)();

 public:
  static void* allocate(size_t n) {
    void* ptr = malloc(n);
    if (ptr == NULL) {
      ptr = oom_malloc(n);
    }
    return ptr;
  }

  static void deallocate(void* p, size_t n) { std::free(p); }

  static void* reallocate(void* p, size_t new_sz) {
    void* ptr = realloc(p, new_sz);
    if (ptr == NULL) {
      ptr = oom_realloc(p, new_sz);
    }
    return ptr;
  }

  static Func set_new_handler(Func func) {
    Func old_handler = oom_handler;
    oom_handler = func;
    return (old_handler);
  }

 private:
  static void* oom_malloc(size_t n);
  static void* oom_realloc(void* p, size_t n);
  static void (*oom_handler)();
};

void (*malloc_alloc::oom_handler)() = nullptr;

void* malloc_alloc::oom_malloc(size_t n) {
  void* res = NULL;
  for (;;) {
    if (oom_handler == NULL) {
      throw std::bad_alloc();
      exit(1);
    }

    (*oom_handler)();
    res = std::malloc(n);
    if (res != NULL) {
      return res;
    }
  }
}

void* malloc_alloc::oom_realloc(void* p, size_t n) {
  void* res = NULL;
  for (;;) {
    if (oom_handler == NULL) {
      throw std::bad_alloc();
      exit(1);
    }

    (*oom_handler)();
    res = std::realloc(p, n);
    if (res != NULL) {
      return res;
    }
  }
}

}  // namespace sgi

#endif  // ALLOCATOR_MALLOC_ALLOC_H_