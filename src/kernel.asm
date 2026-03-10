[BITS 32]

global _start
global problem ;exports the thing form the asm 

extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10 
_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov fs, ax
    mov es, ax
    mov gs, ax
    mov ebp, 0x00200000 ;RAM runtime data, functions, variables etc
    mov esp,ebp
    ;Enable A20 line
    in al, 0x92 ;read from the 92 port
    or al, 2
    out 0x92, al ;write to the port 92

    ;remap the PIC
    cli 
    call remap_pic
    call kernel_main
    
    jmp $ ;jmp forever

times 512-($ - $$) db 0 

problem:
  mov ax, 0 
  div ax

remap_pic:
    mov al, 00010001b
    out 0x20, al ; Tell master PIC

    mov al, 0x20 ; Interrupt 0x20 is where master ISR should start
    out 0x21, al

    mov al, 00000001b
    out 0x21, al
    ret

