#include "kernel.h"
#include "./disk/disk.h"
#include "./e820_mmap/e820_mmap.h"
#include "./memory/heap/kheap.h"
#include "./memory/paging/paging.h"
#include "clock/clock.h"
#include "disk/streamer.h"
#include "drivers/VGA/VGA.h"
#include "fs/pparser.h"
#include "idt/PIC/pic.h"
#include "idt/idt.h"
#include "memory/paging/PMM/pmm.h"

// NOTE: For now keep the Devision by zero error as a halting thing.
// TODO: Maybe add rings to the program
// TODO: Add a PMM

// FILE SYSTEM -> PROCESSES -> PAGING WITH THE PROCESSES
extern void problem();
static pointer_to_page_directory *PD_ptr = 0;
void kernel_main() {
  terminal_initialize();

  PIC_remap(PIC_OFFSET);

  kheap_init();

  // search and init the disk
  disk_search_and_init();

  init_keyboard_buffer();

  idt_init(); // Initialize IDT

  init_bitmap_PMM();
  get_e820_mmap();

  PD_ptr = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT |
                          PAGING_ACCESS_FOROM_ALL);

  // swtich to kernel paging chunk

  paging_switch(PD_ptr->directory_pointer);

  // char *ptr = kzalloc(4096);

  // paging_set(PD_ptr->directory_pointer, (void *)0x1000,
  //            (uint32_t)ptr | PAGING_IS_WRITABLE | PAGING_IS_PRESENT |
  //                PAGING_ACCESS_FOROM_ALL);

  enable_paging();

  enable_interrupts();
  // disk_stream *stream = disk_streamer_new(0);
  // disk_streamer_seek(stream, 0x201);
  // unsigned char *ch = 0;
  // disk_streamer_read(stream, ch, 1, 0);
  // while (1) {
  // }
  terminal_clear();
}
