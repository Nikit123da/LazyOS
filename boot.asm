ORG 0 ;? The origin of the bootloader, memorry adress 0X7c00
BITS 16 ;? type of architecture that the the bootoader uses, 16 bits in our case

; Some bios override the first 33 bytes of data

_start: 
    jmp short start 
    nop ; no operation

times 33 db 0 ; creates 33 bites after the previous jump for the BIOS parameter block, so it wont override data
;makes the code segment 0x7c0
start:
    jmp 0x7c0:step2 

handle_zero: 
    mov ah, 0eh
    mov al, 'Z'
    mov bx, 0x00
    int 0x10
    iret 

handle_one:
    mov ah, 0eh
    mov al, 'V'
    mov bx, 0x00
    int 0x10
    iret 

handle_two:
    mov ah, 0eh
    mov al, 'G'
    mov bx, 0x00
    int 0x10
    iret 


step2: 
    cli ; Clear interrupts, for example if the bios doesn't set the adresses correctly, it'll interfeere with the code and linker, so we account for taht making the origin 0, and we set the registers to 0x7c0, that is starting for a bootloader
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enables interrupts

    mov word[ss:0x00], handle_zero ;first interupt (iterrupt zero)
    mov word[ss:0x02], 0x7c0

    mov word[ss:0x04], handle_one
    mov word[ss:0x06], 0x7c0

    mov word[ss:0x08], handle_two
    mov word[ss:0x0A], 0x7c0

    int 2 ; if we call int 1 it'll call for the second sector of 4 bytes, whuch we assigned the interrupt "handle_one", and if we call int 0 then it'll execute interrupt 0, "handle_zero"

    mov si, message
    call loop_print
    jmp $

loop_print:
    lodsb ;*loads each character that si points to, to the al register, then incriments the si by one to point to the next character
    cmp al, 0
    je .done
    call print_char
    jmp loop_print

.done:
    ret

print_char: 
    mov ah, 0eh ;* the command that outputs A from the al register
    int 0x10 ;*calling the bios rootine, output the character A to the screen
    ret


message: db 'Hello world!', 0 ;* ends with 0 to symbilize the end of the message

times 510- ($ - $$) db 0 ;*says that we need to fill at least 510 bytes if data in our code, if we dont use all the 510 bytes pads with 0's, if we do use all the 510 then nothing happens
dw 0XAA55 ;*

;! commands: 
;nasm -f bin ./boot.asm -o ./boot.bin - compile into binary

;qemu-system-x86_64 -hda ./boot.bin - run in the qemu


