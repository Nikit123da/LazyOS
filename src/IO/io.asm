section .asm

global insb
global insw
global outb
global outw

insb:
  push ebp
  mov ebp,esp
  xor eax, eax ;make it zero

  mov edx, [ebp+8]
  in al, dx ;lower 8 bits of eax loads into al from dx 

  pop ebp
  ret


insw:
  push ebp
  mov ebp, esp
  xor eax,eax

  mov edx, [ebp+8]
  in ax, dx
  pop ebp
  ret 

outb:
  push ebp
  mov ebp,esp
  xor eax, eax

  mov eax, [ebp+12]
  mov edx, [ebp+8]
  out dx, al

  pop ebp
  ret


outw:
  push ebp
  mov ebp,esp
  xor eax, eax

  mov eax, [ebp+12]
  mov edx, [ebp+8]
  out dx, ax

  pop ebp
  ret





