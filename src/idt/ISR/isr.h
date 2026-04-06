#ifndef ISR_H
#define ISR_H

#include <stdint.h>

typedef struct {
  uint32_t ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // pusha
  uint32_t int_no, err_code;                       // Pushed by our ISR
  uint32_t eip, cs, eflags, useresp, ss;           // Pushed by CPU
} registers_t;

void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();

/*
Stack grows down ↓

┌──────────┐  ← stack before interrupt
│ SS       │ Stack Segment
│ SP       │ Stack Pointer(where the code was in its stack)
│ EFLAGS   │ Flags register(carry, zero, interrupt enable, etc.)
│ CS       │ Code Segment
│ IP       │ Instruction Pointer(the exact line that was interrupted)
└──────────┘  ← ESP now points here

*/
void softwear_interrupt_handler(registers_t *reg);
#endif
