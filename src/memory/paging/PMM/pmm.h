#ifndef PMM_H
#define PMM_H

#include <stdint.h>
// 4GB / sizeof(uint8_t) / PAGE_SIZE
#define FREE_PMM_ENTRY 0b00000000
#define WHOLE_ENTRY_TAKN 0b11111111
#define PMM_ENTRIES 131072

void init_bitmap_PMM();
void fill_from_to(uint32_t base, uint32_t len);
int find_free_pmm_space(uint32_t bytes);

#endif
