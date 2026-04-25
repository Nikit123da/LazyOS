ORG 0x7c00 ;? The origin of the bootloader, memorry adress 0X7c00

BITS 16 ;? type of architecture that the the bootoader uses, 16 bits in our case

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
; Some bios override the first 33 bytes of data

;FAT12 header
;The first 3 bytes of the header
jmp short start 
nop ; no operation

OEMIdentifier     db 'LAZYOS  '
BytesPerSector    dw 512 ;512 bytes
SectorsPerCluster db 1
ReservedSectors   dw 150 ;For the kernel
FATCopies         db 2
RoodDirEntries    dw 160
NumSectors        dw 2880
MediaType         db 0xF0
SectorsPerFat     dw 10
SectorsPerTrack   dw 18
NumOfHeads        dw 2
HiddenSectors     dd 0

;Extended BPB
DriveNumber db 0x80
WinNTBit    db 0x00
Signature   db 0x29
VolumeID    dd 0x00
VolumeIDString db 'LAZYOS BOOT'
SystemIDString db 'FAT12   '


start:
    jmp 0:step2 

step2:
    cli
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti

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
;All registers here contain 32 bit.
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


times 510-($ - $$) db 0
dw 0XAA55 

