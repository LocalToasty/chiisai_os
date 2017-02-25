#include "os/memory.hpp"
#include <util/atomic.h>
#include "os/error.hpp"
#include "os/process.hpp"
#include "os/util.hpp"

/*! Describes one chunk of allocated memory.
 *
 * Dynamically allocated memory is managed by a linked list of MemoryChunks.
 * root_chunk points to the head of the list.
 * The tail of the list is a MemoryChunk which has next set to null.
 *
 * The memory corresponding to a chunk begins right after the struct itself and
 * continues until the beginning to the next chunk.
 * Each chunk can either be unused, which means that it can be allocated by new
 * if it has the required size, or used, which means it has already been
 * allocated.
 */
struct MemoryChunk {
  //! True if and only if the chunk is already allocated.
  bool used{false};

  //! The owner of the chunk.
  os::process::Pid owner{nullptr};

  /*! The next chunk of memory.
   *
   * If next is null, then this is the last, empty chunk.
   */
  MemoryChunk* next{nullptr};

  /*! Returns the size of the memory in this chunk.
   *
   * If this chunk is the last chunk, the returned size is 0.
   */
  size_t size();

  /*! Returns a pointer to the first unused chunk with at least the specified
   * size.
   *
   * If there is no such chunk, a pointer to the last, empty chunk is returned.
   */
  MemoryChunk* get_first_unused(size_t min_size);

  /*! Returns a pointer to the previous chunk.
   *
   * If there is no previous chunk (i.e. this is the first chunk), null is
   * returned.
   */
  MemoryChunk* get_previous();

  /*! Merges this chunk with the surrounding ones if they are unused.
   *
   * This function assumes the chunk is unused.
   */
  void merge_with_surrounding();

  /*! Tries to split this chunk into two, with one having a specified size.
   *
   * \returns a pointer to a MemoryChunk with a capacity of at least size bytes.
   */
  MemoryChunk* try_to_partition(size_t size);
};

constexpr size_t minimum_excess_memory_to_partition = sizeof(int);

extern char* __malloc_heap_start;
//! Pointer to the beginning of the heap.
MemoryChunk* root_chunk = (MemoryChunk*)__malloc_heap_start;

//! The last chunk of the heap.
MemoryChunk* last_chunk = root_chunk;

void os::memory::init() {
  *root_chunk = MemoryChunk();
}

uint8_t* os::memory::top_of_heap() {
  return (uint8_t*)last_chunk + sizeof(MemoryChunk);
}

void* operator new(size_t count) {
  MemoryChunk* chunk;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    chunk = root_chunk->get_first_unused(count);

    if (!chunk->next) {
      // there was no free unused chunk of memory; create a new one
      chunk->next =
          (MemoryChunk*)((uint8_t*)chunk + sizeof(MemoryChunk) + count);
      if ((uint8_t*)chunk->next + sizeof(MemoryChunk) >
          os::process::beginning_of_process_stacks()) {
        os::error::error(os::error::out_of_memory);
      }

      *chunk->next = MemoryChunk();
      last_chunk = chunk->next;
    } else {
      // if an already existing block is reused, try to only use a part of it
      chunk->try_to_partition(count);
    }

    chunk->used = true;
    chunk->owner = os::process::get_pid();
  }
  // return the address of the usable memory, situated behind the data structure
  return (void*)((uint8_t*)chunk + sizeof(MemoryChunk));
}

void* operator new[](size_t count) {
  return operator new(count);
}

void operator delete(void* ptr) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // get the actual data structure right before the usable memory
    MemoryChunk* handler = (MemoryChunk*)((uint8_t*)ptr - sizeof(MemoryChunk));
    handler->used = false;

    // combine w/ prev / next segments if they are unused
    handler->merge_with_surrounding();
  }
}

size_t MemoryChunk::size() {
  if (!next) {
    return 0;
  }

  return (size_t)next - (size_t)this - sizeof(MemoryChunk);
}

MemoryChunk* MemoryChunk::get_first_unused(size_t min_size) {
  for (MemoryChunk* chunk = root_chunk; chunk; chunk = chunk->next) {
    if (!chunk->used && (chunk->next == nullptr || chunk->size() >= min_size)) {
      return chunk;
    }
  }

  os::util::unreachable();
}

MemoryChunk* MemoryChunk::get_previous() {
  MemoryChunk* prev = nullptr;
  for (MemoryChunk* chunk = root_chunk; chunk != this; chunk = chunk->next) {
    prev = this;
  }

  return prev;
}

void MemoryChunk::merge_with_surrounding() {
  os::util::assert(!used);

  if (!next->used) {
    next = next->next;
  }
  MemoryChunk* prev = get_previous();
  if (prev && !prev->used) {
    prev->next = next;
  }
}

MemoryChunk* MemoryChunk::try_to_partition(size_t size) {
  os::util::assert(this->next && size <= this->size());

  if (size + sizeof(MemoryChunk) + minimum_excess_memory_to_partition <=
      this->size()) {
    MemoryChunk* tmp = (MemoryChunk*)(next - size - sizeof(MemoryChunk));
    *tmp = MemoryChunk();
    tmp->next = this->next;
    this->next = tmp;

    return tmp;
  }

  return this;
}
