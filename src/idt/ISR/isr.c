#include "isr.h"
#include "../../IO/io.h"
#include "../../drivers/VGA/VGA.h"
/*Intel 32 255 interrupts:
 * https://www.intel.com/content/dam/support/us/en/documents/processors/pentium4/sb/25366821.pdf*/

char *exception_messages[] = {"[0x00] Divide by Zero Exception",
                              "[0x01] Debug Exception",
                              "[0x02] Unhandled Non-maskable Interrupt",
                              "[0x03] Breakpoint Exception",
                              "[0x04] Overflow Exception",
                              "[0x05] Bound Range Exceeded Exception",
                              "[0x06] Invalid Opcode/Operand Exception",
                              "[0x07] Device Unavailable Exception",
                              "[0x08] Double Fault",
                              "[0x09] Coprocessor Segment Overrun",
                              "[0x0A] Invalid TSS Exception",
                              "[0x0B] Absent Segment Exception",
                              "[0x0C] Stack-segment Fault",
                              "[0x0D] General Protection Fault",
                              "[0x0E] Page Fault ",
                              "[0x0F] Inexplicable Error",
                              "[0x10] x87 Floating Exception",
                              "[0x11] Alignment Check",
                              "[0x12] Machine Check",
                              "[0x13] SIMD Floating Exception",
                              "[0x14] Intel reserved",
                              "[0x15] Intel reserved",
                              "[0x16] Intel reserved",
                              "[0x17] Intel reserved",
                              "[0x18] Intel reserved",
                              "[0x19] Intel reserved",
                              "[0x1A] Intel reserved",
                              "[0x1B] Intel reserved",
                              "[0x1C] Intel reserved",
                              "[0x1D] Intel reserved",
                              "[0x1E] Intel reserved",
                              "[0x1F] Intel reserved"};

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

void softwear_interrupt_handler(registers_t *reg) {
  if (reg->int_no < 32) {
    print(exception_messages[reg->int_no]);
    print("\n");
  }
  __asm__ __volatile__("cli;hlt");
}
