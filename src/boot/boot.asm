ORG 0x7c00 ; The origin of the bootloader, memorry adress 0X7c00
BITS 16 ; type of architecture that the the bootoader uses, 16 bits in our case

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start


_start:
  jmp short start 
  nop ; no operation

;FAT12 header
;The first 3 bytes of the header

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
    call do_e820

  .load_protected:
      cli ;clear interrupts
      xor eax, eax    ; ← clear eax before using it
      xor ebx, ebx
      xor ecx, ecx
      xor edx, edx
      lgdt[gdt_descriptor]
      mov eax, cr0
      or eax, 0x1
      mov cr0, eax
      jmp CODE_SEG:load32

[BITS 16]
;-----------------E820-----------------
do_e820:
    mov ax, 0x1234
    mov [0x8000], ax     ; Write a magic number to the count loc
    mov eax, 0xDEADBEEF
    mov [0x8010], eax    ; Write a magic number to the entry loc
    ret
  ;   mov edi, 0x7F010; location that memory map will be stored to
  ;   xor ebx, ebx; ebx must be 0 to start
  ;   xor bp, bp; keep an entry count in bp
  ;   mov edx, 0x0534D4150; Place "SMAP" into edx
  ;   mov eax, 0xe820
  ;
  ;   mov dword [es:di + 20], 1; force a valid ACPI 3.X entry
  ;   mov ecx, 24; ask for 24 bytes
  ;   int 0x15
  ;
  ;   jc nomemmap; carry set on first call means "unsupported function"
  ;   mov edx, 0x0534D4150; Some BIOSes apparently trash this register?
  ;   cmp eax, edx; on success, eax must have been reset to "SMAP"
  ;   jne nomemmap
  ;
  ;   test ebx, ebx; ebx = 0 implies list is only 1 entry long (worthless)
  ;   je nomemmap
  ;
  ;   jmp jmpin
  ;
  ; e820lp:
  ;   mov eax, 0xe820; eax, ecx get trashed on every int 0x15 call
  ;   mov dword [es:di + 20], 1; force a valid ACPI 3.X entry
  ;   mov ecx, 24; ask for 24 bytes again
  ;   int 0x15
  ;   jc memmapend; carry set means "end of list already reached"
  ;   mov edx, 0x0534D4150; repair potentially trashed register
  ; jmpin:
  ;
  ;   jcxz skipent; skip any 0 length entries
  ;   cmp cl, 20; got a 24 byte ACPI 3.X response?
  ;   jbe notext
  ;
  ;   test byte [es:di + 20], 1; if so: is the "ignore this data" bit clear?
  ;   je skipent
  ; notext:
  ;
  ;   mov ecx, [es:di + 8]; get lower dword of memory region length
  ;   test ecx, ecx; is the qword == 0?
  ;   jne goodent
  ;   mov ecx, [es:di + 12]; get upper dword of memory region length
  ;   jecxz skipent; if length qword is 0, skip entry
  ; goodent:
  ;
  ;   inc bp; got a good entry: ++count, move to next storage spot
  ;   add di, 32; Pad to 32 bytes for each record
  ; skipent:
  ;
  ;   test ebx, ebx; if ebx resets to 0, list is complete
  ;   jne e820lp
  ; nomemmap:
  ;
  ; memmapend:
  ;   mov [0x7F000], bp
  ;   xor eax, eax; Create a blank record for termination (32 bytes)
  ;   mov ecx, 8
  ;   rep stosd
  ;   ret
;----------------------------------------------------

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
    mov eax, 1 ;sector we want to load from, second sector 
    mov ecx, 100 ;how many sectors to load
    mov edi, 0x0100000 ;1MB - address we want to load them into
    call ata_lba_read
    jmp CODE_SEG:0X0100000 ;start execuing code from here. BEGINING OF THE KERNEL


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



