#include "kernel.h"
#include "./PIT/pit.h"
#include "./disk/disk.h"
#include "./e820_mmap/e820_mmap.h"
#include "./memory/heap/kheap.h"
#include "./memory/paging/paging.h"
#include "./process/process.h"
#include "drivers/VGA/VGA.h"
#include "idt/PIC/pic.h"
#include "idt/idt.h"
#include "memory/paging/PMM/pmm.h"
#include <stdint.h>

// FILE SYSTEM -> PROCESSES -> PAGING WITH THE PROCESSES
extern void problem();

process *proc_a;
process *proc_b;
void proc_a_entry(void) {
  print("starting process A\n");
  for (int i = 0; i < 10; i++) {
    context_switch(proc_a, proc_b);
    print("A");
    sleep(4000);
    // scheduler(10);
  }
}

void proc_b_entry(void) {
  print("starting process B\n");
  for (int i = 0; i < 10; i++) {
    context_switch(proc_b, proc_a);
    print("B");
    sleep(4000);
    // scheduler(10);
  }
}

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
  int ptr = get_free_physical_address();
  paging_set(PD_ptr->directory_pointer, (void *)0x1000,
             ptr | PAGING_IS_WRITABLE | PAGING_IS_PRESENT |
                 PAGING_ACCESS_FOROM_ALL);
  enable_paging();
  enable_interrupts();
  // disk_stream *stream = disk_streamer_new(0);
  // disk_streamer_seek(stream, 0x201);
  // unsigned char *ch = 0;
  // disk_streamer_read(stream, ch, 1, 0);
  // while (1) {
  // }
  terminal_clear();

  //------------------processes---------------------

  initQueue();
  proc_a = create_process(proc_a_entry);
  proc_b = create_process(proc_b_entry);
  context_switch(NULL, proc_a);
}
