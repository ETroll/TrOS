; sheduling.asm
; Platform dependent helper functions for sheduling

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
    cli
    mov ax, 0x2B      ; Load the index of our TSS structure - The index is
                      ; 0x28, as it is the 5th selector and each is 8 bytes
                      ; long, but we set the bottom two bits (making 0x2B)
                      ; so that it has an RPL of 3, not zero.
    ltr ax            ; Load 0x2B into the task state register.
    sti
    ret

global tss_flush_old
tss_flush_old:
    push ebp
    mov ebp,esp

    cli
	mov eax, 0x2B ;[ebp+8]
	ltr ax
	sti

    mov esp, ebp
    pop ebp
    ret
