#include "heap.h"
#include "../../config.h"
#include "../../status.h"
#include "../memory.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static int heap_validate_table(void *ptr, void *end, heap_table *table) {
  // checks for the heap table to be as many entries as the size of the heap
  // itself
  int res = 0;

  size_t table_size = (size_t)(end - ptr);
  size_t total_blocks = table_size / LAZYOS_HEAP_BLOCK_SIZE;
  if (table->total_entries != total_blocks) {
    res = -EINVARG;
  }

  return res;
}

static bool heap_validate_alignment(void *ptr) {
  return ((unsigned int)ptr % LAZYOS_HEAP_BLOCK_SIZE) == 0;
}

int create_heap(heap *g_heap, void *ptr, void *end, heap_table *table) {
  int res = 0;

  if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end)) {
    res = -EINVARG;
  }

  memset(g_heap, 0, sizeof(*g_heap));
  g_heap->start_addr = ptr;
  g_heap->table = table;
  // NOTE: the address of the table and the start are set correctly in the
  // g_heap

  res = heap_validate_table(ptr, end, table);

  if (!(res < 0)) { // no error
    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total_entries;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);
  }

  return res;
}

static uint32_t heap_aligne_value_to_upper(uint32_t val) {
  if ((val % LAZYOS_HEAP_BLOCK_SIZE) == 0) {
    return val;
  }

  val = (val - (val % LAZYOS_HEAP_BLOCK_SIZE));
  val += LAZYOS_HEAP_BLOCK_SIZE;
  return val;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry) {
  return entry & 0x0f;
}

int heap_get_start_block(heap *g_heap, uint32_t total_blocks) {
  heap_table *table = g_heap->table;
  int current_block = 0;
  int start_block = -1;

  for (size_t i = 0; i < table->total_entries; i++) {
    if (heap_get_entry_type(table->entries[i]) !=
        HEAP_BLOCK_TABLE_ENTRY_FREE) { //
      current_block = 0;
      start_block = -1;
      continue;
    }

    // If first block
    if (start_block == -1) {
      start_block = i;
    }
    current_block++;
    if (current_block == total_blocks) {

      break;
    }
  }

  if (start_block == -1) {
    return -NO_MEMORY;
  }
  return start_block;
}

void *heap_block_to_address(heap *g_heap, uint32_t block) {
  return g_heap->start_addr + (block * LAZYOS_HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_taken(heap *g_heap, uint32_t start_block,
                            uint32_t total_blocks) {
  int end_block = (start_block + total_blocks) - 1;
  HEAP_BLOCK_TABLE_ENTRY entry =
      HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
  if (total_blocks > 1) {
    entry |= HEAP_BLOCK_HAS_NEXT;
  }

  for (int i = start_block; i <= end_block; i++) {
    g_heap->table->entries[i] = entry;
    entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
    if (i != end_block) {
      entry |= HEAP_BLOCK_HAS_NEXT;
    }
  }
}

void *heap_malloc_blocks(heap *g_heap, uint32_t total_blocks) {
  void *address = 0;

  int start_block = heap_get_start_block(g_heap, total_blocks);
  if (start_block < 0) {
    return address;
  }

  address = heap_block_to_address(g_heap, start_block);
  // mark blocks as taken.

  heap_mark_blocks_taken(g_heap, start_block, total_blocks);
  return address;
}

void *heap_malloc(heap *g_heap, size_t size) {
  size_t aligned_size = heap_aligne_value_to_upper(size);
  uint32_t total_blocks = aligned_size / LAZYOS_HEAP_BLOCK_SIZE;

  return heap_malloc_blocks(g_heap, total_blocks);
}

int heap_address_to_block(heap *g_heap,
                          void *address) { // calc which block to free
  return ((int)(address - g_heap->start_addr)) / LAZYOS_HEAP_BLOCK_SIZE;
}

void heap_mark_blocks_free(heap *g_heap, int starting_block) {
  heap_table *table = g_heap->table;
  for (int i = starting_block; i < (int)table->total_entries; i++) {
    HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
    table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
    if (!(entry & HEAP_BLOCK_HAS_NEXT)) {
      break;
    }
  }
}

void heap_free(heap *g_heap, void *ptr) {
  heap_mark_blocks_free(g_heap, heap_address_to_block(g_heap, ptr));
}
