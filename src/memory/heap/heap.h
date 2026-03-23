#ifndef HEAP_H
#define HEAP_H

#include "../../config.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00

#define HEAP_START_ADDRESS 0x01000000
#define PAGE_SIZE 0x01000

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST 0b01000000

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

typedef struct {
  HEAP_BLOCK_TABLE_ENTRY *entries;
  size_t total_entries;
} heap_table;

typedef struct {
  heap_table *table;
  void *start_addr;
} heap;

int create_heap(heap *g_heap, void *ptr, void *end, heap_table *table);

void heap_free(heap *g_heap, void *address);

void *heap_malloc(heap *g_heap, size_t size);

#endif
