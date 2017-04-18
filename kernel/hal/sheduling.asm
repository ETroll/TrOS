; sheduling.asm
; Platform dependent helper functions for sheduling and multitasking
;
;                                +8                +12                 +16
; void enter_usermode(registers_t* reg, unsigned int location, unsigned int userstack);

global thread_enterUsermode
thread_enterUsermode:
    push ebp
    mov ebp,esp

    pusha
    pushf
    mov eax, cr3
    push eax

    mov eax, [ebp+8]
    cmp eax, 0
    je .usermode

    mov [eax+4], ebx
    mov [eax+8], ecx
    mov [eax+12], edx
    mov [eax+16], esi
    mov [eax+20], edi
    ; EAX
    mov ebx, [esp+36]
    mov [eax], ebx
    ; ESP, We want the SP to be pointing at the end of the caller frame
    mov ebx, ebp
    add ebx, 8 ; point to last argument passed to enter_usermode
    mov [eax+24], ebx
    ; EBP, we want to point to the callers EBP. (The value of current ebp)
    mov ebx, [ebp]
    mov [eax+28], ebx
    ; EIP where we want to "land" when we get back.
    mov ebx, [ebp+4]
    mov [eax+32], ebx
    ; EFLAGS
    mov ebx, [esp+4]
    mov [eax+36], ebx
    ; CR3
    pop ebx
    mov [eax+40], ebx

    ; ----- Done saving state
    ; Now on to the important bits
    .usermode:
        mov esp, [ebp+16] ; Set the new stack for the new application to be used
                          ; from here on now
        mov ax, 0x23 ; User mode data selector is 0x20 (GDT entry 3). Also sets RPL to 3 (so 0x23)
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        mov eax, esp

        push 0x23       ; SS, notice it uses same selector as above
        push eax        ; ESP
        pushfd          ; EFLAGS (note pushfd is 32bit, pushf is 64bit "safe")

        ; Lets "fix" some flags
        pop eax
        or eax, 0x200   ; enable IF in EFLAGS
        push eax
        push 0x1b       ; CS, user mode code selector is 0x18. With RPL 3 this is 0x1b

        ; Lets push our new entry point to be used when it "returns" when performing the iret
        push dword [ebp+12] ;note 32 bit address size "hardcoded"

        ;Call the EOI so timer dont stop
        push eax
        mov eax, 0x20
        out 0x20, eax
        pop eax

        iret

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
global scheduler_switchThread
scheduler_switchThread:
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

    ;Call the EOI so timer dont stop
    ; #define PIC1_CREG 0x20
    ; pio_outb(0x20, PIC1_CREG);
    push eax
    mov eax, 0x20
    out 0x20, eax
    pop eax

    ret ;This ends all!

global scheduler_startIdle
scheduler_startIdle:
    mov eax, [esp+4]
    mov ebx, [esp+8]
    mov esp, ebx
    push eax
    ret
