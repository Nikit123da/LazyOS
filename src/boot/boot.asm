ORG 0x7c00 ; The origin of the bootloader, memorry adress 0X7c00
BITS 16 ; type of architecture that the the bootoader uses, 16 bits in our case

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
mmap_ent equ 0x600

jmp short start 
nop ; no operation

;FAT12 header
;The first 3 bytes of the header
;FF - EOF
;each entry is 2 bytes long
; FAT12 BPB - layout: sector 0=boot, 1-200=kernel, 201-2880=FAT data, total=2881
OEMIdentifier           db 'LAZYOS  '
BytesPerSector          dw 512
SectorsPerCluster       db 1
ReservedSectors         dw 201       ; 1 boot + 200 kernel sectors
FATCopies               db 2
RootDirEntries          dw 224
NumSectors              dw 2880      ; total sectors on disk
MediaType               db 0xE8      ; fixed disk
SectorsPerFat           dw 9
SectorsPerTrack         dw 18
NumberOfHeads           dw 2
HiddenSectors           dd 0
SectorsBig              dd 0

;Extended BPB
DriveNumber             db 0x80
WinNTBit                db 0x00
Signature               db 0x29
VolumeID                dd 0xD105
VolumeIDString          db 'LAZYOS BOOT' ;11 bytes
SystemIDString          db 'FAT12   '


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
    mov di, 0x604              ; Buffer location: ES:0x604
    xor ebx, ebx               ; Start fresh — ebx=0 means "first call"
    xor bp, bp                 ; bp counts entries we've kept
    mov edx, 0x0534D4150       ; "SMAP" magic
    mov eax, 0xe820            ; Function code
    mov [es:di + 20], dword 1  ; Force ACPI 3.x "valid" bit (some BIOSes ignore)
    mov ecx, 24                ; Buffer is 24 bytes
    int 0x15                   ; Call BIOS

    jc short .failed           ; Carry set on FIRST call = "E820 not supported"
    mov edx, 0x0534D4150       ; Restore SMAP (some BIOSes clobber edx)
    cmp eax, edx               ; eax should now be "SMAP"
    jne short .failed
    test ebx, ebx              ; ebx = 0 means "list is just 1 entry" — useless
    je short .failed
    jmp short .jmpin           ; Skip past the loop setup to entry processing

.e820lp:
    mov eax, 0xe820                    ; eax gets trashed, restore it
    mov [es:di + 20], dword 1          ; Re-prime the valid bit
    mov ecx, 24                        ; Re-prime buffer size
    int 0x15
    jc short .e820f                    ; Carry now = "end of list" (not error)
    mov edx, 0x0534D4150               ; Restore SMAP 

.jmpin:
    jcxz .skipent              ; If cx = 0, BIOS wrote nothing → skip
    cmp cl, 20                 ; Did BIOS write 24 bytes (ACPI 3.x) or just 20?
    jbe short .notext          ; If ≤ 20, no extended attributes — just process it
    test byte [es:di + 20], 1  ; Otherwise check the "valid" bit
    je short .skipent          ; If valid bit clear → BIOS says ignore this entry

.notext:
    mov ecx, [es:di + 8]       ; Low 32 bits of length
    or ecx, [es:di + 12]       ; OR with high 32 bits
    jz .skipent                ; If length is 0 → skip
    inc bp                     ; Good entry — increment count
    add di, 24                 ; Move buffer pointer to next slot

.skipent:
    test ebx, ebx              ; If ebx = 0, BIOS signaled "this was the last"
    jne short .e820lp          ; Otherwise loop

.e820f:
    mov [es:mmap_ent], bp      ; Save final entry count to memory
    clc                        ; Clear carry → success
    ret

.failed:
    stc                        ; Set carry → failure
    ret 
;----------------------------------------------------

; GDT
gdt_start:
  ;a must in the GDT struct https://wiki.osdev.org/GDT_Tutorial
  gdt_null: ;NULL descriptor
      dq 0x0

  ; offset 0x8

  gdt_code: ; code descriptor
      dw 0xfffff ; segment limit first 0-15 bits LIMIT
      dw 0      ; base first 0 -15 bits BASE 
      db 0      ; Base 16 -23 bits
      db 0x9a   ;Acess byte (10011010) (1) present (00) DPL (1) type (code or data) (1) executable (0) direction grows up (1) R/W (write never allowed) (0) Acess bit
      db 11001111b ;High 4 bit flags and the low 4 bit flags
      db 0        ; Base 24-31 bits BASE 

  ; offset 0x10 

  gdt_data:       ; DS, SS, ES , FS , GS. data descriptor
      dw 0xfffff ; segment limit first 0-15 bits
      dw 0      ; base first 0 -15 bits
      db 0      ; Base 16 -23 bits
      db 0x92   ;Acess byte (10010010)
      db 11001111b ;High 4 bit flags and the low 4 bit flags (1) Gnaluar (1) 32 bit (0) not long mode (0) reserved / padding
      db 0        ; Base 24-31 bits

  ;Can have TSS (task state segment descriptor) totaly optional
gdt_end:


gdt_descriptor:
    dw gdt_end-gdt_start-1 ;Size of the gdt
    dd gdt_start ;Base address of the gdt


[BITS 32]
load32:
    mov eax, 1 ;sector we want to load from, second sector 
    mov ecx, 200 ;how many sectors to load
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
    mov al, 0x20 ;read
    out dx, al

  ;Read all secotrs to the memory

  .next_sector:
    push ecx

  ;check if we need to read
  .try_again:
    mov dx, 0x1f7
    in al, dx
    test al, 8 ;checks the 3rd bit for if data ready
    jz .try_again

  ;need to read 256 words at a time
    mov ecx, 256 ;512 bytes
    mov dx, 0x1f0
    rep insw ;reads 256 words from I/O port in dx into edi register
    pop ecx
    loop .next_sector
    ;End of reading sectors into memory
    ret


times 510-($ - $$) db 0
dw 0XAA55 



