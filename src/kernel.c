#include "kernel.h"
#include "./disk/disk.h"
#include "./memory/heap/kheap.h"
#include "./memory/paging/paging.h"
#include "drivers/VGA/VGA.h"
#include "idt/PIC/pic.h"
#include "idt/idt.h"
#include <stdint.h>
// TODO: Next is to initialized the PMM or the HEAP.
// NOTE: For now keep the Devision by zero error as a halting thing.
// TODO: Maybe add rings to the program

// FILE SYSTEM -> PROCESSES -> PAGING WITH THE PROCESSES

extern void problem();
static pointer_to_page_directory *PD_ptr = 0;
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
  init_keyboard_buffer();

  print("Initializing IDT\n");
  idt_init(); // Initialize IDT
  print("IDT has been initialized\n");
  // problem();
  PD_ptr = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT |
                          PAGING_ACCESS_FOROM_ALL);
  // swtich to kernel paging chunk
  paging_switch(PD_ptr->directory_pointer);

  // char *ptr = kzalloc(4096);
  // paging_set(PD_ptr->directory_pointer, (void *)0x1000,
  //            (uint32_t)ptr | PAGING_IS_WRITABLE | PAGING_IS_PRESENT |
  //                PAGING_ACCESS_FOROM_ALL);
  //
  // enable_paging();
  //
  // char *ptr2 = (char *)0x1000;
  // ptr2[0] = 'A';
  // ptr2[1] = 'B';
  // // ptr[2] = '\n';
  // print(ptr2);
  // print(ptr);

  char buff[512];
  disk_read_secotr(0, 1, buff);

  print(buff);
  enable_interrupts();
  // void *ptr = kmalloc(50);
  // void *ptr2 = kmalloc(5000);
  // void *ptr3 = kmalloc(5600);
  // kfree(ptr);
  // void *ptr4 = kmalloc(50);
  // void *ptr5 = kmalloc(50);
  // if (ptr || ptr2 || ptr3 || ptr4 || ptr5) {
  // }
  terminal_initialize();
}
