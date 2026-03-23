section .asm

[BITS 32]

; global - from asm to c
; extern - from c to asm

global idt_load

idt_load:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    cli
    lidt [ebx]
    pop ebp
    ret

; Macro wrappers for the softwear interrupts
%macro int_wrapper 2
  global %1
  extern %2
  %1:
    pushad
    push esp          ; Pass stack frame to handler
    call %2
    add esp, 4
    popad
    add esp, 4        ; Skip error code (if pushed by CPU)
    iret
%endmacro

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

;Macro wrappers creation
;int_wrapper handle_zero_wrapper, idt_zero  

hardwear_interrupts no_interrupt, no_interrupt_handler
hardwear_interrupts handle_keyboard_interrupt, keyboard_interrupt 
;hardwear_interrupts handle_timer_interrupt, timer_interrupt

;Wrapper funcion export
