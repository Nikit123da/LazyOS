#include "kheap.h"
#include "../../config.h"
#include "../../drivers/VGA/VGA.h"
#include "../../status.h"
#include "../memory.h"
#include "heap.h"

// 100MB heap / block size (4096)
heap kernel_heap;
heap_table kernel_heap_table;

void kheap_init() {
  uint32_t total_table_entries =
      LAZYOS_HEAP_SIZE_BYTES / LAZYOS_HEAP_BLOCK_SIZE;
  kernel_heap_table.entries =
      (HEAP_BLOCK_TABLE_ENTRY *)(LAZYOS_HEAP_TABLE_ADDRESS);
  kernel_heap_table.total_entries = total_table_entries;

  void *end = (void *)(LAZYOS_HEAP_ADDRESS + LAZYOS_HEAP_SIZE_BYTES);
  int res = create_heap(&kernel_heap, (void *)(LAZYOS_HEAP_ADDRESS), end,
                        &kernel_heap_table);

  if (res < 0) {
    print("Error Initiilizing the heap");
    return;
  }

  print("Heap Initialized \n");
}

void *kmalloc(size_t size) { return heap_malloc(&kernel_heap, size); }

void kfree(void *ptr) { heap_free(&kernel_heap, ptr); }

void *kzalloc(size_t size) {
  // fills the alocatad space with dummy zeors
  void *ptr = kmalloc(size);
  if (!ptr)
    return 0;

  memset(ptr, 0x00, size);
  return ptr;
}
