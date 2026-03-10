#include "drivers/VGA/VGA.h"
#include "idt/idt.h"
// FIX: The keyboard interrupt doesnt work after diviosion by zero interrupt.

extern void problem();

void kernel_main() {
  terminal_initialize();
  print("Hello world\ngoing down\n");
  idt_init(); // Initialize IDT
  print("IDT has been initialized\n");
  // problem();
}
