#ifndef E820_MMAP_H
#define E820_MMAP_H

#include <stdint.h>

typedef struct {
  uint64_t base;
  uint64_t length;
  uint32_t type;
  uint32_t extended;
  uint32_t padding[2]; // This accounts for the 'add di, 32
} __attribute__((packed)) E820Entry;
void get_e820_mmap();

#endif // !E820_MMAP_H
