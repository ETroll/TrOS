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

global switch_task
switch_task:
    pusha
    pushf
    mov %cr3, %eax ;Push CR3
    push %eax
    mov 44(%esp), %eax ;The first argument, where to save
    mov %ebx, 4(%eax)
    mov %ecx, 8(%eax)
    mov %edx, 12(%eax)
    mov %esi, 16(%eax)
    mov %edi, 20(%eax)
    mov 36(%esp), %ebx ;EAX
    mov 40(%esp), %ecx ;IP
    mov 20(%esp), %edx ;ESP
    add $4, %edx ;Remove the return value ;)
    mov 16(%esp), %esi ;EBP
    mov 4(%esp), %edi ;EFLAGS
    mov %ebx, (%eax)
    mov %edx, 24(%eax)
    mov %esi, 28(%eax)
    mov %ecx, 32(%eax)
    mov %edi, 36(%eax)
    pop %ebx ;CR3
    mov %ebx, 40(%eax)
    push %ebx ;Goodbye again ;)
    mov 48(%esp), %eax ;Now it is the new object
    mov 4(%eax), %ebx ;EBX
    mov 8(%eax), %ecx ;ECX
    mov 12(%eax), %edx ;EDX
    mov 16(%eax), %esi ;ESI
    mov 20(%eax), %edi ;EDI
    mov 28(%eax), %ebp ;EBP
    push %eax
    mov 36(%eax), %eax ;EFLAGS
    push %eax
    popf
    pop %eax
    mov 24(%eax), %esp ;ESP
    push %eax
    mov 44(%eax), %eax ;CR3
    mov %eax, %cr3
    pop %eax
    push %eax
    mov 32(%eax), %eax ;EIP
    xchg (%esp), %eax ;We do not have any more registers to use as tmp storage
    mov (%eax), %eax ;EAX
    ret ;This ends all!
