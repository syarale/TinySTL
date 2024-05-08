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
class DefaultAlloc {
 public:
  static void* Allocate(size_t bytes) {
    assert(bytes > 0);

    // use malloc_alloc to allocate larger memory
    if (bytes > MAX_BYTES) {
      return sgi::MallocAlloc::Allocate(bytes);
    }

    bytes = RoundUp(bytes);
    size_t index = FreeListsIndex(bytes);
    obj* free_head = free_lists_[index];
    if (free_head == nullptr) {
      void* p = Refill(bytes);
      return p;
    }

    free_lists_[index] = free_head->next_free_obj;
    free_head->next_free_obj = nullptr;
    return free_head;
  }

  static void Deallocate(void* p, size_t bytes) {
    if (bytes > MAX_BYTES) {
      return MallocAlloc::Deallocate(p, bytes);
    }

    bytes = RoundUp(bytes);
    size_t index = FreeListsIndex(bytes);

    obj* free_head = free_lists_[index];
    static_cast<obj*>(p)->next_free_obj = free_head;
    free_lists_[index] = static_cast<obj*>(p);
  }

  static void* Reallocate(void* p, size_t old_sz, size_t new_sz) {
    if (old_sz > MAX_BYTES && new_sz > MAX_BYTES) {
      return std::realloc(p, new_sz);
    }

    if (RoundUp(old_sz) == RoundUp(new_sz)) {
      return p;
    }

    void* new_p = Allocate(new_sz);
    size_t copy_size = old_sz < new_sz ? old_sz : new_sz;
    memmove(new_p, p, copy_size);
    Deallocate(p, old_sz);
    return new_p;
  }

 private:
  union obj {
    obj* next_free_obj;
    char data[1];
  };

  static size_t RoundUp(size_t bytes) {
    return (bytes + ALIGN - 1) & (~(ALIGN - 1));
  }

  static size_t FreeListsIndex(size_t bytes) {
    return (bytes + ALIGN - 1) / ALIGN - 1;
  }

  static char* AllocChunk(size_t bytes, int& objs);  // nobj may change
  static char* Refill(size_t bytes);

  static obj* free_lists_[NFREELISTS];

  // memory pool
  static char* start_free_;
  static char* end_free_;
  static size_t extend_heap_size_;
};

DefaultAlloc::obj* DefaultAlloc::free_lists_[NFREELISTS] = {nullptr};
char* DefaultAlloc::start_free_ = nullptr;
char* DefaultAlloc::end_free_ = nullptr;
size_t DefaultAlloc::extend_heap_size_ = 0;

char* DefaultAlloc::AllocChunk(size_t bytes, int& nobjs) {
  size_t total_bytes = bytes * nobjs;
  size_t bytes_left = end_free_ - start_free_;

  char* res = start_free_;
  if (bytes_left >= total_bytes) {
    start_free_ += total_bytes;
    return res;
  }

  if (bytes_left >= bytes) {
    nobjs = bytes_left / bytes;
    total_bytes = nobjs * bytes;
    start_free_ += total_bytes;
    return res;
  }

  // memory pool expansion: add the remaining space to
  // free_lists_ and then apply for memory
  assert(bytes_left % ALIGN == 0);
  int index = FreeListsIndex(bytes_left);
  obj* curr = reinterpret_cast<obj*>(start_free_);
  curr->next_free_obj = free_lists_[index];
  free_lists_[index] = curr;
  start_free_ = end_free_ = nullptr;

  size_t bytes_to_get = 2 * total_bytes + RoundUp(extend_heap_size_ >> 4);
  start_free_ = static_cast<char*>(std::malloc(bytes_to_get));
  if (start_free_ == nullptr) {
    for (int i = bytes; i <= MAX_BYTES; i += ALIGN) {
      int index = FreeListsIndex(i);
      obj* ptr = free_lists_[index];
      if (ptr != nullptr) {
        free_lists_[index] = ptr->next_free_obj;
        start_free_ = reinterpret_cast<char*>(ptr);
        end_free_ = start_free_ + i;
        return AllocChunk(bytes, nobjs);
      }
    }
    // Go to malloc's OOM handler
    return static_cast<char*>(MallocAlloc::Allocate(total_bytes));
  }
  end_free_ = start_free_ + bytes_to_get;
  return AllocChunk(bytes, nobjs);
}

char* DefaultAlloc::Refill(size_t bytes) {
  int nobjs = DEFAULT_CHUNKS;
  char* start = AllocChunk(bytes, nobjs);  // nobjs may change

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

  int index = FreeListsIndex(bytes);
  free_lists_[index] = reinterpret_cast<obj*>(start)->next_free_obj;
  return start;
}

}  // namespace sgi

#endif  // ALLOCATOR_DEFAULT_ALLOC_H_