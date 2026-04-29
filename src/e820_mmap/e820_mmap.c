#include "e820_mmap.h"
#include "../memory/paging/PMM/pmm.h"
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

void get_e820_mmap() {
  uint16_t cnt = *(uint16_t *)0x7F000;
  E820Entry *entries = (E820Entry *)0x7F010;
  for (int i = 0; i < cnt; i++) { // 0x20 jmps for the structs
    uint64_t base = entries[i].base;
    uint64_t len = entries[i].length;
    uint32_t type = entries[i].type;

    if (type != 1) { // not free
      fill_from_to(base, len);
    }
  }
}
