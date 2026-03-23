#include "kernel.h"
#include "./memory/heap/kheap.h"
#include "./memory/paging/paging.h"
#include "drivers/VGA/VGA.h"
#include "idt/PIC/pic.h"
#include "idt/idt.h"
#include <stdint.h>
// TODO: Next is to initialized the PMM or the HEAP.
// NOTE: For now keep the Devision by zero error as a halting thing.

extern void problem();
static paging_4gb_chunk *kernel_chunk = 0;
void kernel_main() {
  terminal_initialize();

  print("Remaping PIC...\n");
  PIC_remap(PIC_OFFSET);
  print("PIC has been remapped\n");

  // print("Initializing Paging...\n");
  // // init_paging();
  // print("Paging has been initied\n");// Initialize heap
  print("Initializing the heap...\n");
  kheap_init();
  print("Heap has been initialized\n");

  print("Initializing IDT\n");
  idt_init(); // Initialize IDT
  print("IDT has been initialized\n");
  // problem();
  kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT |
                                PAGING_ACCESS_FOROM_ALL);
  // swtich to kernel paging chunk
  paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

  enable_paging();

  // void *ptr = kmalloc(50);
  // void *ptr2 = kmalloc(5000);
  // void *ptr3 = kmalloc(5600);
  // kfree(ptr);
  // void *ptr4 = kmalloc(50);
  // void *ptr5 = kmalloc(50);
  // if (ptr || ptr2 || ptr3 || ptr4 || ptr5) {
  // }
}
