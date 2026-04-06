[BITS 32]

section .asm


global enable_paging
global paging_load_directory

;cr3 bits 12-31 is the PDBR registor to store the address of the Page Directory
paging_load_directory:
  push ebp
  mov ebp, esp 
  mov eax, [ebp + 8]
  mov cr3, eax
  pop ebp
  ret


enable_paging:
  push ebp
  mov ebp, esp

  mov eax, cr0
  or eax, 0x80000001

  mov cr0, eax
  pop ebp
  ret



