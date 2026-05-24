#ifndef PAGING_H
#define PAGING_H

#include <stdbool.h>
#include <stdint.h>
#define PAGING_CACHE_DISABLED 0b00010000
#define PAGING_WRITE_THROUGH 0b00001000
#define PAGING_ACCESS_FOROM_ALL 0b00000100 // Supriviser bit
#define PAGING_IS_WRITABLE 0b00000010
#define PAGING_IS_PRESENT 0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

// First N directory entries are shared from the kernel directory into every
// process directory. Each entry covers 4 MB, so 32 entries = 128 MB, which
// is enough to cover the first 1 MB, the kernel image, and the kernel heap.
#define KERNEL_SHARED_PD_ENTRIES 32

typedef struct {

  uint32_t *directory_pointer;

} pointer_to_page_directory;

void paging_switch(uint32_t *directory);

pointer_to_page_directory *paging_new_4gb(uint32_t flags);
pointer_to_page_directory *paging_new_process_directory(uint32_t *kernel_directory);

void enable_paging();

int paging_set(uint32_t *directory, void *vertual_addr, uint32_t val);
int paging_get_indexes(void *virtual_address, uint32_t *directory_index_out,
                       uint32_t *table_index_out);
bool paging_is_aligned(void *addr);
int vmm_alloc(uint32_t *directory, uint32_t virt_start, uint32_t size,
              uint32_t flags);
int free_vmm(uint32_t *directory, void *virt_start, uint32_t size);
#endif
