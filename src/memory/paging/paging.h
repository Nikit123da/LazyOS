#ifndef PAGING_H
#define PAGING_H

#include <stddef.h>
#include <stdint.h>
#define PAGING_CACHE_DISABLED 0b00010000
#define PAGING_WRITE_THROUGH 0b00001000
#define PAGING_ACCESS_FOROM_ALL 0b00000100 // Supriviser bit
#define PAGING_IS_WRITABLE 0b00000010
#define PAGING_IS_PRESENT 0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

typedef struct {

  uint32_t *directory_entry;

} paging_4gb_chunk;

void paging_switch(uint32_t *directory);

paging_4gb_chunk *paging_new_4gb(uint32_t flags);

void enable_paging();

uint32_t *paging_4gb_chunk_get_directory(paging_4gb_chunk *chunk);

#endif
