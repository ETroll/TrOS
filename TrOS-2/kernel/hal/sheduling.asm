; sheduling.asm
; Platform dependent helper functions for sheduling and multitasking

global enter_usermode
enter_usermode:
    push ebp
    mov ebp,esp

    cli
    mov ax, 0x23 ; user mode data selector is 0x20 (GDT entry 3). Also sets RPL to 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x23       ; SS, notice it uses same selector as above
    push esp        ; ESP
    pushfd          ; EFLAGS

    pop eax
    or eax, 0x200   ; enable IF in EFLAGS
    push eax

    push 0x1b       ; CS, user mode code selector is 0x18. With RPL 3 this is 0x1b
    lea eax, [.a]   ; EIP first
    push eax
    iretd
    .a:
        add esp, 4  ;fix stack

    mov esp, ebp
    pop ebp
    ret

global tss_flush
tss_flush:
    push ebp
    mov ebp,esp

    cli
    mov eax, [ebp+8]
    ltr ax
    sti

    mov esp, ebp
    pop ebp
    ret

; http://wiki.osdev.org/Kernel_Multitasking
; http://www.imada.sdu.dk/Courses/DM18/Litteratur/IntelnATT.htm
global process_switch
process_switch:
    pusha
    pushf
    ;mov %cr3, %eax ;Push CR3
    mov eax, cr3
    ;push %eax
    push eax
    ;mov 44(%esp), %eax ;The first argument, where to save
    mov eax, [esp+44]
    ;mov %ebx, 4(%eax)
    ;mov %ecx, 8(%eax)
    ;mov %edx, 12(%eax)
    ;mov %esi, 16(%eax)
    ;mov %edi, 20(%eax)
    mov [eax+4], ebx
    mov [eax+8], ecx
    mov [eax+12], edx
    mov [eax+16], esi
    mov [eax+20], edi

    ;mov 36(%esp), %ebx ;EAX
    ;mov 40(%esp), %ecx ;IP
    ;mov 20(%esp), %edx ;ESP
    mov ebx, [esp+36]
    mov ecx, [esp+40]
    mov edx, [esp+20]

    ;add $4, %edx ;Remove the return value ;)
    add edx, 4
    ;mov 16(%esp), %esi ;EBP
    mov esi, [esp+16]
    ;mov 4(%esp), %edi ;EFLAGS
    mov edi, [esp+4]
    ;mov %ebx, (%eax)
    ;mov %edx, 24(%eax)
    ;mov %esi, 28(%eax)
    ;mov %ecx, 32(%eax)
    ;mov %edi, 36(%eax)

    mov [eax], ebx
    mov [eax+24], edx
    mov [eax+28], esi
    mov [eax+32], ecx
    mov [eax+36], edi

    ;pop %ebx ;CR3
    pop ebx
    ;mov %ebx, 40(%eax)
    mov [eax+40], ebx
    ;push %ebx ;Goodbye again ;)
    push ebx
    ;mov 48(%esp), %eax ;Now it is the new object
    ;mov 4(%eax), %ebx ;EBX
    ;mov 8(%eax), %ecx ;ECX
    ;mov 12(%eax), %edx ;EDX
    ;mov 16(%eax), %esi ;ESI
    ;mov 20(%eax), %edi ;EDI
    ;mov 28(%eax), %ebp ;EBP

    mov eax, [esp+48]
    mov ebx, [eax+4]
    mov ecx, [eax+8]
    mov edx, [eax+12]
    mov esi, [eax+16]
    mov edi, [eax+20]
    mov ebp, [eax+28]


    ;push %eax
    push eax
    ;mov 36(%eax), %eax ;EFLAGS
    mov eax, [eax+36]
    ;push %eax
    push eax
    popf
    ;pop %eax
    pop eax
    ;mov 24(%eax), %esp ;ESP
    mov esp, [eax+24]
    ;push %eax
    push eax
    ;mov 44(%eax), %eax ;CR3
    mov eax, [eax+44]
    ;mov %eax, %cr3
    mov cr3, eax
    ;pop %eax
    pop eax
    ;push %eax
    push eax
    ;mov 32(%eax), %eax ;EIP
    mov eax, [eax+32]
    ;xchg (%esp), %eax ;We do not have any more registers to use as tmp storage
    xchg eax, [esp]
    ;mov (%eax), %eax ;EAX
    mov eax, [eax]
    ret ;This ends all!
