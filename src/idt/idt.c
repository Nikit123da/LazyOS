#include "idt.h"
#include "../IO/io.h"
#include "../PIT/pit.h"
#include "../config.h"
#include "../drivers/VGA/VGA.h"
#include "../memory/memory.h"
#include <stdint.h>
// NOTE: masked = disabled
//  NOTE: I need a wrapper around the interrupt because I need to do an iret. Im
//   not sure why.

// NOTE: extern keyword - bring a routine from assembly to C

struct idt_desc
    idt_descriptors[LAZYOS_TOTAL_INTERRUPTS]; // 512 descriptor entries
struct idtr_desc idtr_descriptor;

extern void
idt_load(struct idtr_desc *ptr); // helps debugging to put the struct

/*WRAPPERS*/

extern void handle_zero_wrapper();
extern void handle_keyboard_interrupt();
extern void handle_timer_interrupt() {};
extern void no_interrupt();

/*HANDLER FUNCTIONS*/

void idt_zero() {
  print("Devide by zero error\n");
  outb(0x20, 0x20); // should have sent a signal to port 20 that the int ha been
                    // handled, clearly doesn't
}
void keyboard_interrupt() {
  print("Keyboard pressed\n");
  // terminal_clear();
  // outb(0x20, 0x20);
}

void no_interrupt_handler() { outb(0x20, 0x20); } // val 00100000
void timer_interrupt() { init_pit(1000); }

void idt_set(int interrupt_num, void *address) {
  struct idt_desc *desc = &idt_descriptors[interrupt_num];
  desc->offset_1 = (uint32_t)address & 0x0000ffff;
  desc->selector = KERNEL_CODE_SELECTOR;
  desc->zero = 0x00;
  desc->type_attributes =
      0x8E; // set the value to 10001110 as the first 0 is the second offset.
            // see https://wiki.osdev.org/Interrupt_Descriptor_Table Gate
            // Descrptor tag (10001110)
  desc->offset_2 = (uint32_t)address >> 16; // higher part of the offset
}

void idt_init() {
  memset(idt_descriptors, 0, sizeof(idt_descriptors));
  idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
  idtr_descriptor.base = (uint32_t)idt_descriptors;

  for (int i = 0; i < LAZYOS_TOTAL_INTERRUPTS; i++) {
    idt_set(i, no_interrupt);
  }

  idt_set(0, handle_zero_wrapper);
  // idt_set(0x20, handle_timer_interrupt);
  idt_set(0x21, handle_keyboard_interrupt);

  // load the idt_desc
  idt_load(&idtr_descriptor);
  asm("sti"); // enable interrupts after initializing the IDT fully/
}
