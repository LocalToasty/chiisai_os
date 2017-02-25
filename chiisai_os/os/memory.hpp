#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <stddef.h>
#include <stdint.h>

//! Allocates a continuous block of count bytes of memory.
void* operator new(size_t count);
void* operator new[](size_t count);

//! Deletes a previously allocated block of memory.
void operator delete(void* ptr);
void operator delete[](void* ptr);

namespace os {
namespace memory {
void init();

//! Returns the first address not used by the heap.
uint8_t* top_of_heap();
}
}

#endif /* MEMORY_HPP */