global context_switch

SP_OFFSET       equ 0
VIRT_OFFSET     equ 4
STATE_OFFSET    equ 8
PROCESS_RUNNING equ 3

context_switch:
    mov eax, [esp + 4];curr
    mov edx, [esp + 8];next

    ; --- save current process ---
    test eax, eax
    jz .load

    pusha
    mov [eax + SP_OFFSET], esp

.load:
    ; --- update next state ---
    mov dword [edx + STATE_OFFSET], PROCESS_RUNNING

    ; --- switch page directory if needed ---
    mov ebx, [edx + VIRT_OFFSET]
    test ebx, ebx            ; if virt_addr is NULL skip cr3 switch
    jz .skip_cr3
    mov ecx, cr3
    cmp ebx, ecx
    je .skip_cr3
    mov cr3, ebx

.skip_cr3:
    ; --- restore next process ---
    mov esp, [edx + SP_OFFSET]   ; load next->sp into esp
    popa                          ; restore all registers
    sti
    ret                           ; jump to saved eip

