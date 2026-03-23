#include "idt.h"
#include "../IO/io.h"
#include "../PIT/pit.h"
#include "../config.h"
#include "../drivers/VGA/VGA.h"
#include "../drivers/keyboard/keyboard.h"
#include "../memory/memory.h"
#include "./ISR/isr.h"
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

extern void handle_keyboard_interrupt();
extern void handle_timer_interrupt() {};
extern void no_interrupt();

/*HANDLER FUNCTIONS*/

void keyboard_interrupt() { keyboard_callback(); }

void no_interrupt_handler() { outb(0x20, 0x20); } // val 00100000
// void timer_interrupt() { init_pit(1000); }

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
  idt_set(0x0, isr0);
  idt_set(0x1, isr1);
  idt_set(0x2, isr2);
  idt_set(0x3, isr3);
  idt_set(0x4, isr4);
  idt_set(0x5, isr5);
  idt_set(0x6, isr6);
  idt_set(0x7, isr7);
  idt_set(0x8, isr8);
  idt_set(0x9, isr9);
  idt_set(0xA, isr10);
  idt_set(0xB, isr11);
  idt_set(0xC, isr12);
  idt_set(0xD, isr13);
  idt_set(0xE, isr14);
  idt_set(0xF, isr15);
  idt_set(0x010, isr16);
  idt_set(0x011, isr17);
  idt_set(0x012, isr18);
  idt_set(0x013, isr19);
  idt_set(0x014, isr20);
  idt_set(0x015, isr21);
  idt_set(0x016, isr22);
  idt_set(0x017, isr23);
  idt_set(0x018, isr24);
  idt_set(0x019, isr25);
  idt_set(0x01A, isr26);
  idt_set(0x01B, isr27);
  idt_set(0x01C, isr28);
  idt_set(0x01D, isr29);
  idt_set(0x01E, isr30);
  idt_set(0x01F, isr31);

  // idt_set(0x20, handle_timer_interrupt);
  idt_set(0x21, handle_keyboard_interrupt);

  // load the idt_desc
  idt_load(&idtr_descriptor);
  asm("sti"); // enable interrupts after initializing the IDT fully/
}
