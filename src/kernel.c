#include "kernel.h"
#include "./PIT/pit.h"
#include "./disk/disk.h"
#include "./e820_mmap/e820_mmap.h"
#include "./fs/fat/fat12.h"
#include "./memory/heap/kheap.h"
#include "./memory/paging/paging.h"
#include "./process/process.h"
#include "disk/streamer.h"
#include "drivers/VGA/VGA.h"
#include "idt/PIC/pic.h"
#include "idt/idt.h"
#include "memory/paging/PMM/pmm.h"
#include "str/str.h"
#include <stdint.h>

extern void problem();
int shared_variable = 0;
uint32_t shared_lock = 0;

void proc_a_entry(void) {
  print("\nstarting process A\n");
  setLock(&shared_lock); // lock the lock

  for (int i = 0; i < 50; i++) {
    print("A");
    shared_variable++; // modify the data
    print_digit(shared_variable);
    sleep(4000); // outside the lock — good
  }
  releaseLock(&shared_lock); // unlock the lock
}

void proc_b_entry(void) {
  print("\nstarting process B\n");
  // setLock(&shared_lock); // lock the lock

  for (int i = 0; i < 50; i++) {
    print("B");
    // shared_variable--; // modify the data
    // print_digit(shared_variable);
    sleep(4000);
  }
  // releaseLock(&shared_lock); // unlock the lock
}

void proc_c_entry(void) {
  print("\nstarting process C\n");
  for (int i = 0; i < 100; i++) {
    print("C");
    int something = 5 / 0;
    if (something)
      sleep(4000);
  }
}

pointer_to_page_directory *PD_ptr = 0;
void kernel_main() {
  terminal_initialize();
  PIC_remap(PIC_OFFSET);
  kheap_init();
  disk_search_and_init();
  init_keyboard_buffer();
  idt_init(); // Initialize IDT
  init_bitmap_PMM();
  get_e820_mmap();
  PD_ptr = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT |
                          PAGING_ACCESS_FOROM_ALL);
  paging_switch(PD_ptr->directory_pointer);
  enable_paging();

  FAT12_table_init();

  init_pit(100);
  enable_interrupts();
  terminal_clear();

  initQueue();
  create_process(proc_a_entry);
  create_process(proc_b_entry);
  create_process(proc_c_entry);
  start_processes();
  print("\nall processes done, back in kernel_main\n");
  // problem();
}
