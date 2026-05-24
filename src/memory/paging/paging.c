#include "paging.h"
#include "../../status.h"
#include "../heap/kheap.h"
#include "./PMM/pmm.h"
#include <stdint.h>

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
int paging_set(uint32_t *directory, void *virtual_addr,
               uint32_t physical_addr) {
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

  // Lazily allocate a page table the first time anything in this 4 MB region
  // is mapped in this directory.
  if (!(entry & PAGING_IS_PRESENT)) {
    uint32_t *new_table =
        kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    if (!new_table) {
      return -NO_MEMORY;
    }
    directory[directory_index] = (uint32_t)new_table | PAGING_IS_PRESENT |
                                 PAGING_IS_WRITABLE | PAGING_ACCESS_FOROM_ALL;
    entry = directory[directory_index];
  }

  uint32_t *table = (uint32_t *)(entry & 0xfffff000);
  table[table_index] = physical_addr;

  return 0;
}

/*
 * Build a fresh page directory for a new process.
 *
 * Shares the first KERNEL_SHARED_PD_ENTRIES entries with the kernel directory
 * (so kernel code, heap, BIOS area, etc. stay reachable after CR3 switches).
 * The remaining entries are zero, leaving the rest of the 4 GB free for the
 * process to map its own pages into. Page tables for those regions are
 * allocated on demand by paging_set.
 */
pointer_to_page_directory *
paging_new_process_directory(uint32_t *kernel_directory) {
  uint32_t *directory =
      kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
  if (!directory) {
    return 0;
  }

  for (int i = 0; i < KERNEL_SHARED_PD_ENTRIES; i++) { // NOTE: WHY 32 SHARED?
    directory[i] = kernel_directory[i];
  }

  pointer_to_page_directory *ptr = kzalloc(sizeof(pointer_to_page_directory));
  if (!ptr) {
    return 0;
  }
  ptr->directory_pointer = directory;
  return ptr;
}

int vmm_alloc(uint32_t *directory, uint32_t virt_start, uint32_t size,
              uint32_t flags) {
  if (!paging_is_aligned(((void *)virt_start))) {
    return -EINVARG;
  }

  uint32_t pages = (size + PAGING_PAGE_SIZE - 1) / PAGING_PAGE_SIZE;

  for (uint32_t i = 0; i < pages; i++) {
    uint32_t virt = virt_start + i * PAGING_PAGE_SIZE;
    int phys = get_free_physical_address();
    if (phys == -NO_MEMORY)
      return 0; // out of memory

    paging_set(directory, (void *)virt,
               phys | flags | PAGING_IS_PRESENT | PAGING_IS_WRITABLE);
  }

  return virt_start;
}

int free_vmm(uint32_t *directory, void *virt_start, uint32_t size) {
  if (!paging_is_aligned(virt_start)) {
    return -EINVARG;
  }

  uint32_t pages = (size + PAGING_PAGE_SIZE - 1) / PAGING_PAGE_SIZE;

  for (uint32_t i = 0; i < pages; i++) {
    uint32_t virt = (uint32_t)virt_start + i * PAGING_PAGE_SIZE;

    uint32_t dir_idx = 0;
    uint32_t tbl_idx = 0;
    int res = paging_get_indexes((void *)virt, &dir_idx, &tbl_idx);
    if (res < 0)
      return res;

    uint32_t pd_entry = directory[dir_idx];
    if (!(pd_entry & PAGING_IS_PRESENT))
      continue;

    uint32_t *table = (uint32_t *)(pd_entry & 0xFFFFF000);
    uint32_t pt_entry = table[tbl_idx];

    if (pt_entry & PAGING_IS_PRESENT) {
      free_physical_frame(pt_entry & 0xFFFFF000);
      table[tbl_idx] = 0x0;
      asm volatile("invlpg (%0)" : : "r"(virt) : "memory"); // reset the TLB
    }
  }

  return 0;
}
