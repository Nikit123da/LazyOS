[BITS 32]

global setLock
global releaseLock
extern executing_process

SPINNING_OFFSET equ 18 ;the offset in the process struct

setLock:
    push ebp
    mov ebp, esp
    mov edx, [ebp + 8]            ; address of the lock
    mov ecx, [executing_process]
    mov byte [ecx + SPINNING_OFFSET], 1 ;setting the proecss as spining
.spin:
    mov eax, 1
    xchg eax, [edx]              ; atomic test-and-set
    test eax, eax                ;If proc is in a spin state aka 1.
    jnz .spin                    ; still held -> keep waiting (in asm)

    mov ecx, [executing_process]
    mov byte [ecx + SPINNING_OFFSET], 0 ;set the spin value to 0 again in the proc struct

    pop ebp
    ret

releaseLock:
    push ebp
    mov ebp, esp
    mov edx, [ebp + 8]           ; edx = the lock variable
    mov dword [edx], 0           ; free it
    pop ebp
    ret
