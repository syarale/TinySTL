#ifndef ALLOCATOR_DEFAULT_ALLOC_H_
#define ALLOCATOR_DEFAULT_ALLOC_H_

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>

#include "malloc_alloc.h"

namespace sgi {

inline constexpr int ALIGN = 8;
inline constexpr int MAX_BYTES = 128;
inline constexpr int NFREELISTS = MAX_BYTES / ALIGN;
inline constexpr int DEFAULT_CHUNKS = 20;

// second level memory allocator
class default_alloc {
 public:
  static void* allocate(size_t bytes) {
    assert(bytes > 0);

    // use malloc_alloc to allocate larger memory
    if (bytes > MAX_BYTES) {
      return sgi::malloc_alloc::allocate(bytes);
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

char* default_alloc::alloc_chunk(size_t bytes, int& nobjs) {
  size_t total_bytes = bytes * nobjs;
  size_t bytes_left = end_free - start_free;

  char* res = start_free;
  if (bytes_left >= total_bytes) {
    start_free += total_bytes;
    return res;
  }

  if (bytes_left >= bytes) {
    nobjs = bytes_left / bytes;
    total_bytes = nobjs * bytes;
    start_free += total_bytes;
    return res;
  }

  // memory pool expansion: add the remaining space to
  // free_lists and then apply for memory
  assert(bytes_left % ALIGN == 0);
  int index = free_lists_index(bytes_left);
  obj* curr = reinterpret_cast<obj*>(start_free);
  curr->next_free_obj = free_lists[index];
  free_lists[index] = curr;
  start_free = end_free = nullptr;

  size_t bytes_to_get = 2 * total_bytes + round_up(extend_heap_size >> 4);
  start_free = static_cast<char*>(std::malloc(bytes_to_get));
  if (start_free == nullptr) {
    for (int i = bytes; i <= MAX_BYTES; i += ALIGN) {
      int index = free_lists_index(i);
      obj* ptr = free_lists[index];
      if (ptr != nullptr) {
        free_lists[index] = ptr->next_free_obj;
        start_free = reinterpret_cast<char*>(ptr);
        end_free = start_free + i;
        return alloc_chunk(bytes, nobjs);
      }
    }
    // Go to malloc's OOM handler
    return static_cast<char*>(malloc_alloc::allocate(total_bytes));
  }
  end_free = start_free + bytes_to_get;
  return alloc_chunk(bytes, nobjs);
}

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

}  // namespace sgi

#endif  // ALLOCATOR_DEFAULT_ALLOC_H_