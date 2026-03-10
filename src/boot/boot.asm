ORG 0x7c00 ;? The origin of the bootloader, memorry adress 0X7c00

BITS 16 ;? type of architecture that the the bootoader uses, 16 bits in our case

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
; Some bios override the first 33 bytes of data

_start: 
    jmp short start 
    nop ; no operation

times 33 db 0 ; creates 33 bites after the previous jump for the BIOS parameter block, so it wont override data
;makes the code segment 0x7c0
start:
    jmp 0:step2 

;handle_zero: 
;    mov ah, 0eh
;    mov al, 'Z'
;    mov bx, 0x00
;    int 0x10
;    iret 

;handle_one:
;    mov ah, 0eh
;    mov al, 'V'
;    mov bx, 0x00
;    int 0x10
;    iret 

step2;
    cli ; Clear interrupts, for example if the bios doesn't set the adresses correctly, it'll interfeere with the code and linker, so we account for taht making the origin 0, and we set the registers to 0x7c0, that is starting for a bootloader
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enables interrupts

    ;mov word[ss:0x00], handle_zero ;first interupt (iterrupt zero)
    ;mov word[ss:0x02], 0x7c0

  ;int 2 ; if we call int 1 it'll call for the second sector of 4 bytes, whuch we assigned the interrupt "handle_one", and if we call int 0 then it'll execute interrupt 0, "handle_zero"

  .load_protected:
      cli ;clear interrupts
      lgdt[gdt_descriptor]
      mov eax, cr0
      or eax, 0x1
      mov cr0, eax
      jmp CODE_SEG:load32


; GDT
gdt_start:
  ;a must in the GDT struct https://wiki.osdev.org/GDT_Tutorial
  gdt_null:
      dq 0x0

  ; offset 0x8

  gdt_code:
      dw 0xffff ; segment limit first 0-15 bits
      dw 0      ; base first 0 -15 bits
      db 0      ; Base 16 -23 bits
      db 0x9a   ;Acess byte
      db 11001111b ;High 4 bit flags and the low 4 bit flags
      db 0        ; Base 24-31 bits

  ; offset 0x10 

  gdt_data:       ; DS, SS, ES , FS , GS
      dw 0xffff ; segment limit first 0-15 bits
      dw 0      ; base first 0 -15 bits
      db 0      ; Base 16 -23 bits
      db 0x92   ;Acess byte
      db 11001111b ;High 4 bit flags and the low 4 bit flags
      db 0        ; Base 24-31 bits

gdt_end:


gdt_descriptor:
    dw gdt_end-gdt_start-1
    dd gdt_start


[BITS 32]
load32:
    mov eax, 1 ;sector we want to load from second sector 
    mov ecx, 100 ;how many sectors to load
    mov edi, 0x0100000 ;1MB - address we want to load them into
    call ata_lba_read
    jmp CODE_SEG:0X0100000 ;start execuing code from here.


ata_lba_read:
;https://stackoverflow.com/questions/3215878/what-are-in-out-instructions-in-x86-used-for
;;All registers here contain 32 bit.
    mov ebx, eax ;Backup the LBA 
    ; send the highest 8 bits of the lba to the hard disk controller
    shr eax, 24 ;shift to the right, after the shift contain highest LBA
    or eax, 0xE0 ;Selects the master drive
    mov dx, 0x1F6 ;port address
    out dx, al 
      ;Finished sendng the highest bits of LBA

    ;Send total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ;Finished total sectors to read

    ;Send more bits of the LBA
    mov eax, ebx
    mov dx, 0x1F3
    out dx, al
    ;Finished sending more bits of the LBA

    
    ;Send more bits of the LBA
    mov dx, 0x1F4
    mov eax, ebx ;Restore the backup LBA
    shr eax, 8 
    out dx, al
    ;Finished sending more bits of the LBA

    ; Send upper 16 bits of the LBA
    mov dx, 0x1F5
    mov eax, edx
    shr eax, 16
    out dx, al
    ;Finished sending upper bits of the LBA

    mov dx, 0x1F7
    mov al, 0x20
    out dx, al

  ;Read all secotrs to the memory

  .next_sector:
    push ecx

  ;check if we need to read
  .try_again:
    mov dx, 0x1f7
    in al, dx
    test al, 8
    jz .try_again

  ;need to read 256 words at a time
    mov ecx, 256
    mov dx, 0x1f0
    rep insw ;reads a word from I/O port in dx into edi register
    pop ecx
    loop .next_sector
    ;End of reading sectors into memory
    ret


times 510-($ - $$) db 0 ;*says that we need to fill at least 510 bytes if data in our code, if we dont use all the 510 bytes pads with 0's, if we do use all the 510 then nothing happens
dw 0XAA55 ;*

;buffer: 

;! commands: 
;nasm -f bin ./boot.asm -o ./boot.bin - compile into binary

;qemu-system-x86_64 -hda ./boot.bin - run in the qemu

;target remote | qemu-system-x86_64 -hda ./boot.bin -S -gdb stdio

;bless <file to see its machine code>
