#include "paging.h"
#include "../heap/kheap.h"
#include <stdint.h>

void paging_load_directory(uint32_t *directory);
static uint32_t *current_directory = 0;

paging_4gb_chunk *
paging_new_4gb(uint32_t flags) { // for now put the paging lineary, so virtual
                                 // 0x40000 points to physical 0x40000
  uint32_t *directory =
      kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
  int offset = 0;
  for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {

    uint32_t *entry =
        kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++) {
      entry[j] =
          (offset + (j * PAGING_PAGE_SIZE)) | flags; // linear paging formula
    }
    offset +=
        (PAGING_TOTAL_ENTRIES_PER_TABLE *
         PAGING_PAGE_SIZE); // the offset of each page table 0x400000 (400k)
    directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITABLE;
  }

  paging_4gb_chunk *chunk_4gb = kzalloc(sizeof(paging_4gb_chunk));
  chunk_4gb->directory_entry = directory;
  return chunk_4gb;
}

void paging_switch(uint32_t *directory) {
  paging_load_directory(directory);
  current_directory = directory;
}

uint32_t *paging_4gb_chunk_get_directory(paging_4gb_chunk *chunk) {
  return chunk->directory_entry;
}
