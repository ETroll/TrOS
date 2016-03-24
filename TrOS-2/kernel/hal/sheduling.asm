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
global process_switch
process_switch:
    pusha
    pushf
    mov eax, cr3 ;Push CR3
    push eax
    mov eax, [esp+44] ;The first argument, where to save
    mov [eax+4], ebx
    mov [eax+8], ecx
    mov [eax+12], edx
    mov [eax+16], esi
    mov [eax+20], edi

    mov ebx, [esp+36] ;EAX
    mov ecx, [esp+40] ;EIP
    mov edx, [esp+20] ;ESP

    add edx, 4 ;Remove the return value
    mov esi, [esp+16] ;EBP
    mov edi, [esp+4];EFLAGS

    mov [eax], ebx
    mov [eax+24], edx
    mov [eax+28], esi
    mov [eax+32], ecx
    mov [eax+36], edi

    pop ebx ;CR3
    mov [eax+40], ebx
    push ebx

    mov eax, [esp+48] ;Now it is the new object
    mov ebx, [eax+4]
    mov ecx, [eax+8]
    mov edx, [eax+12]
    mov esi, [eax+16]
    mov edi, [eax+20]
    mov ebp, [eax+28]

    push eax
    mov eax, [eax+36] ;EFLAGS
    push eax
    popf
    pop eax
    mov esp, [eax+24] ;ESP
    push eax
    mov eax, [eax+40] ;CR3
    ;xchg bx, bx ; BOCHS Magic debugger

    mov cr3, eax ; Set CR3 to provided pagedir
    ; BOOM!
    pop eax
    push eax
    mov eax, [eax+32] ;EIP
    xchg eax, [esp] ;We do not have any more registers to use as tmp storage
    mov eax, [eax] ;EAX
    ret ;This ends all!
