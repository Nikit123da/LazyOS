#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>
#include <stdint.h>

#define HEAP_START_ADDRESS 0x02000000
#define PAGE_SIZE 0x01000

typedef struct DList DList;

struct DList {
  uint32_t length;
  struct DList *next;
  struct DList *prev;
  bool isFree;
};

void init_heap(uint32_t num_of_pages);

void free(void *address);

void *malloc(uint32_t size);

void *calloc(uint32_t elements, uint32_t elements_size);

void *realloc(void *address, uint32_t size);

#endif
