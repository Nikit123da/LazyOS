[BITS 32]

global setLock
global releaseLock

setLock:
    push ebp
    mov ebp, esp
    mov edx, [ebp + 8]      ; address of the lock
    ; cli
.spin:
    mov eax, 1
    xchg eax, [edx]
    test eax, eax
    jnz .spin
    pop ebp
    ret

releaseLock:
    push ebp
    mov ebp, esp
    mov edx, [ebp + 8] //edx = the lcok variable 
    mov dword [edx], 0      ; free it
    ; sti
    pop ebp
    ret
