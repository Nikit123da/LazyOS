section .asm

[BITS 32]

%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push byte 0          ; Push dummy error code
    push byte %1         ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Macro for exceptions WITH error code
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push byte %1         ; Push interrupt number (CPU already pushed error code)
    jmp isr_common_stub
%endmacro

ISR_NOERRCODE 0     
ISR_NOERRCODE 1    
ISR_NOERRCODE 2    
ISR_NOERRCODE 3    
ISR_NOERRCODE 4    
ISR_NOERRCODE 5    
ISR_NOERRCODE 6    
ISR_NOERRCODE 7    
ISR_ERRCODE   8    
ISR_NOERRCODE 9    
ISR_ERRCODE   10   
ISR_ERRCODE   11   
ISR_ERRCODE   12  
ISR_ERRCODE   13   
ISR_ERRCODE   14   
ISR_NOERRCODE 15  
ISR_NOERRCODE 16   
ISR_ERRCODE   17   
ISR_NOERRCODE 18  
ISR_NOERRCODE 19  
ISR_NOERRCODE 20  
ISR_ERRCODE   21   
ISR_NOERRCODE 22   
ISR_NOERRCODE 23  
ISR_NOERRCODE 24   
ISR_NOERRCODE 25   
ISR_NOERRCODE 26   
ISR_NOERRCODE 27   
ISR_NOERRCODE 28   
ISR_NOERRCODE 29   
ISR_ERRCODE   30   
ISR_NOERRCODE 31   

; Common ISR stub - builds registers_t struct
extern softwear_interrupt_handler

isr_common_stub:
    ; Already pushed: ss, esp, eflags, cs, eip, err_code, int_no
    
    pusha              ; Push edi, esi, ebp, esp, ebx, edx, ecx, eax
    
    mov ax, ds
    push eax           ; Push ds
    
    mov ax, 0x10       ; Load kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push esp           ; Push pointer to registers_t struct
    call softwear_interrupt_handler
    add esp, 4         ; Clean up pushed ESP
    
    pop eax            ; Restore original data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa               ; Restore general purpose registers
    add esp, 8         ; Clean up pushed error code and int number
    sti
    iret               ; Return from interrupt
