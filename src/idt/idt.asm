section .asm

[BITS 32]

; global - from asm to c
; extern - from c to asm

global idt_load
global enable_interrupts

enable_interrupts:
  sti
  ret

idt_load:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    cli
    lidt [ebx]
    pop ebp
    ret


%macro hardwear_interrupts 2
  global %1
  extern %2
  %1:
    pushad
    call %2
    popad
    mov al, 0x20
    out 0x20, al
    iret
%endmacro


hardwear_interrupts no_interrupt, no_interrupt_handler
hardwear_interrupts irq1, keyboard_interrupt 
hardwear_interrupts irq0, timer_interrupt

;Wrapper funcion export
