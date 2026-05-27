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
    mov ebp, 0x00200000 ;The stack
    mov esp,ebp

    ;Enable A20 line
    in al, 0x92 ;read from the 92 port
    or al, 2
    out 0x92, al ;write to the port 92

    cli 
    call kernel_main
    jmp $ ;jmp forever

times 512-($ - $$) db 0 

problem:
  mov ax, 0 
  div ax

