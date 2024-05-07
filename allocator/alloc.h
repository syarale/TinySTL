#ifndef ALLOCATOR_ALLOC_H_
#define ALLOCATOR_ALLOC_H_

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>

namespace sgi {

using size_t = std::size_t;

inline constexpr int ALIGN = 8;
inline constexpr int MAX_BYTES = 128;
inline constexpr int NFREELISTS = MAX_BYTES / ALIGN;
inline constexpr int DEFAULT_CHUNKS = 20;

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

// second level memory allocator
class default_alloc {
 public:
  static void* allocate(size_t bytes) {
    assert(bytes > 0);

    // use malloc_alloc to allocate larger memory
    if (bytes > MAX_BYTES) {
      return malloc_alloc::allocate(bytes);
    }

    bytes = round_up(bytes);
    size_t index = free_lists_index(bytes);
    obj* free_head = free_lists[index];
    if (free_head == nullptr) {
      void* p = refill(bytes);
      return p;
    }

    free_lists[index] = free_head->next_free_obj;
    free_head->next_free_obj = nullptr;
    return free_head;
  }

  static void deallocate(void* p, size_t bytes) {
    if (bytes > MAX_BYTES) {
      return malloc_alloc::deallocate(p, bytes);
    }

    bytes = round_up(bytes);
    size_t index = free_lists_index(bytes);

    obj* free_head = free_lists[index];
    static_cast<obj*>(p)->next_free_obj = free_head;
    free_lists[index] = static_cast<obj*>(p);
  }

  static void* reallocate(void* p, size_t old_sz, size_t new_sz) {
    if (old_sz > MAX_BYTES && new_sz > MAX_BYTES) {
      return std::realloc(p, new_sz);
    }

    if (round_up(old_sz) == round_up(new_sz)) {
      return p;
    }

    void* new_p = allocate(new_sz);
    size_t copy_size = old_sz < new_sz ? old_sz : new_sz;
    memmove(new_p, p, copy_size);
    deallocate(p, old_sz);
    return new_p;
  }

 private:
  union obj {
    obj* next_free_obj;
    char data[1];
  };

  static size_t round_up(size_t bytes) {
    return (bytes + ALIGN - 1) & (~(ALIGN - 1));
  }

  static size_t free_lists_index(size_t bytes) {
    return (bytes + ALIGN - 1) / ALIGN - 1;
  }

  static char* alloc_chunk(size_t bytes, int& objs);  // nobj may change
  static char* refill(size_t bytes);

  static obj* free_lists[NFREELISTS];

  // memory pool
  static char* start_free;
  static char* end_free;
  static size_t extend_heap_size;
};

default_alloc::obj* default_alloc::free_lists[NFREELISTS] = {nullptr};
char* default_alloc::start_free = nullptr;
char* default_alloc::end_free = nullptr;
size_t default_alloc::extend_heap_size = 0;

char* default_alloc::alloc_chunk(size_t bytes, int& nobjs) {}

char* default_alloc::refill(size_t bytes) {
  int nobjs = DEFAULT_CHUNKS;
  char* start = alloc_chunk(bytes, nobjs);  // nobjs may change

  if (nobjs == 1) {
    return start;
  }

  obj* prev = reinterpret_cast<obj*>(start);
  prev->next_free_obj = nullptr;
  for (int i = 1; i < nobjs; i++) {
    obj* curr = reinterpret_cast<obj*>(start + i * bytes);
    curr->next_free_obj = nullptr;
    prev->next_free_obj = curr;
    prev = curr;
  }

  int index = free_lists_index(bytes);
  free_lists[index] = reinterpret_cast<obj*>(start)->next_free_obj;
  return start;
}

// In fact, __USE_MALLOC_ALLOC macro is never defined in SGI-STL
#ifdef __USE_MALLOC_ALLOC
using alloc = malloc_alloc;
#else
using alloc = default_alloc;
#endif  //  __USE_MALLOC_ALLOC

template <typename T, typename Alloc>
class allocator {
 public:
  static T* allocate(size_t n) {
    return n <= 0 ? nullptr : static_cast<T*>(Alloc::allocate(n * sizeof(T)));
  }

  static T* allocate(void) {
    return static_cast<T*>(Alloc::allocate(sizeof(T)));
  }

  static void deallocate(T* p, size_t n) {
    if (n != 0) {
      Alloc::deallocate(p, n * sizeof(T));
    }
  }

  static void deallocate(T* p) { Alloc::deallocate(p, sizeof(T)); }
};

}  // namespace sgi
#endif  // ALLOCATOR_ALLOC_H_
