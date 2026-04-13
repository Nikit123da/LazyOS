#include "paging.h"
#include "../../status.h"
#include "../heap/kheap.h"
#include <stdint.h>
// TODO: clock algirithm for paging

extern void paging_load_directory(uint32_t *directory);
static uint32_t *current_directory = 0;

/*
 * building the page_directory, page_table and filling them up with linear
 * aligment and returning the
 */
pointer_to_page_directory *
paging_new_4gb(uint32_t flags) { // for now put the paging lineary, so virtual
                                 // 0x40000 points to physical 0x40000
  uint32_t *directory = kzalloc(
      sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE); // Page Directory
  int offset = 0;

  for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {

    uint32_t *entry = kzalloc(sizeof(uint32_t) *
                              PAGING_TOTAL_ENTRIES_PER_TABLE); // Page Table

    for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE;
         j++) { // filling page table
      entry[j] =
          (offset + (j * PAGING_PAGE_SIZE)) | flags; // linear paging formula
    }

    offset +=
        (PAGING_TOTAL_ENTRIES_PER_TABLE *
         PAGING_PAGE_SIZE); // the offset of each page table 0x400000 (400k)
    directory[i] =
        (uint32_t)entry | flags | PAGING_IS_WRITABLE; // filing page directory
  }

  pointer_to_page_directory *ptr = kzalloc(sizeof(pointer_to_page_directory));
  ptr->directory_pointer =
      directory; // putting the address of the directory in the struct
  return ptr;    // return the struct
}

void paging_switch(uint32_t *directory) {
  paging_load_directory(directory);
  current_directory = directory;
}

bool paging_is_aligned(void *addr) {
  return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
}

/*
  calcing the physical address from the virtual address
*/
int paging_get_indexes(void *virtual_address, uint32_t *directory_index_out,
                       uint32_t *table_index_out) {
  int res = 0;
  if (!(paging_is_aligned(virtual_address))) {
    res = -EINVARG;
    return res;
  }

  *directory_index_out = (uint32_t)virtual_address /
                         (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
  *table_index_out = (uint32_t)virtual_address %
                     (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) /
                     PAGING_PAGE_SIZE;

  return res;
}

/*
  sets the page table entry to the value which is the addres i guess
*/
int paging_set(uint32_t *directory, void *virtual_addr, uint32_t val) {
  if (!paging_is_aligned(virtual_addr)) {
    return -EINVARG;
  }

  uint32_t directory_index = 0;
  uint32_t table_index = 0;

  int res = paging_get_indexes(virtual_addr, &directory_index, &table_index);
  if (res < 0) {
    return res;
  }

  uint32_t entry = directory[directory_index];
  uint32_t *table =
      (uint32_t *)(entry &
                   0xfffff000); // 20 most right bits, which is the address, now
                                // we need to set the flags.
  table[table_index] = val;

  return res;
}
