#ifndef E820_MMAP_H
#define E820_MMAP_H

#include <stdint.h>

typedef struct {
  uint64_t base;
  uint64_t length;
  uint32_t type;
  uint32_t extended;
} __attribute__((packed)) E820Entry;
void get_e820_mmap();

#endif // !E820_MMAP_H
