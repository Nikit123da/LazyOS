#include "heap.h"
#include <stdint.h>
void *heap_start;
void *heap_end;
bool is_heap_initialized = false;

// STEP: Init the heap

void init_heap(uint32_t num_of_pages) {
  if (is_heap_initialized) {
    heap_start = (void *)HEAP_START_ADDRESS;
  }
}
