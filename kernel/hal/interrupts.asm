;
; Helper methods for IRQ handling

extern irq_default_handler
extern syscall_dispatcher

global gdt_load
gdt_load:
    mov eax, [esp+4]
    lgdt [eax]

    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush

    .flush:
        ret

global idt_load
idt_load:
    mov eax, [esp+4]
    lidt [eax]
    ret

%macro IRQ_NOERRCODE 1
    global isr%1
    isr%1:
        push byte 0
        push byte %1
        jmp irq_common
%endmacro

%macro IRQ_ERRCODE 1
   global isr%1
    isr%1:
        push byte %1
        jmp irq_common
%endmacro

%macro IRQ 2
  global irq%1
  irq%1:
    push byte %1
    push byte %2
    jmp irq_common
%endmacro

;rename: isr_fallback
global irq_fallback
irq_fallback:
    push byte 0
    push byte 0x0F

irq_common:
    pusha
    mov ax, ds              ; Lower 16-bits of eax = ds.
    push eax                ; Save the data segment descriptor

    mov ax, 0x10            ; Load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_default_handler

    pop ebx                 ; Reload the original data segment descriptor
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa
    add esp, 8              ; Cleans up the pushed error code and pushed IRQ number
    iret                    ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP


IRQ_NOERRCODE 0
IRQ_NOERRCODE 1
IRQ_NOERRCODE 2
IRQ_NOERRCODE 3
IRQ_NOERRCODE 4
IRQ_NOERRCODE 5
IRQ_NOERRCODE 6
IRQ_NOERRCODE 7
IRQ_ERRCODE   8
IRQ_NOERRCODE 9
IRQ_ERRCODE   10
IRQ_ERRCODE   11
IRQ_ERRCODE   12
IRQ_ERRCODE   13
IRQ_ERRCODE   14
IRQ_NOERRCODE 15
IRQ_NOERRCODE 16
IRQ_NOERRCODE 17
IRQ_NOERRCODE 18
IRQ_NOERRCODE 19
IRQ_NOERRCODE 20
IRQ_NOERRCODE 21
IRQ_NOERRCODE 22
IRQ_NOERRCODE 23
IRQ_NOERRCODE 24
IRQ_NOERRCODE 25
IRQ_NOERRCODE 26
IRQ_NOERRCODE 27
IRQ_NOERRCODE 28
IRQ_NOERRCODE 29
IRQ_NOERRCODE 30
IRQ_NOERRCODE 31
; IRQ_NOERRCODE 128
IRQ 0,  32
IRQ 1,  33
IRQ 2,  34
IRQ 3,  35
IRQ 4,  36
IRQ 5,  37
IRQ 6,  38
IRQ 7,  39
IRQ 8,  40
IRQ 9,  41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

global syscall_isr
syscall_isr:
    push ebp
    mov ebp, esp
    sub esp, 4

    push ebx
    push ds
    push es
    push fs
    push gs

    mov [ebp-4], dword 0x10
    mov ds, [ebp-4]
    mov es, [ebp-4]
    mov fs, [ebp-4]
    mov gs, [ebp-4]

    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    call syscall_dispatcher
    pop edi
    pop esi
    pop edx
    pop ecx
    add esp, 8  ; Dont want to pop into eax, or ebx (stored on stack previous)
                ; since it is used for return value.
                ; Just want to "dispose of" the old value
    pop ebx
    mov gs, bx
    pop ebx
    mov fs, bx
    pop ebx
    mov es, bx
    pop ebx
    mov ds, bx
    pop ebx     ; Ref above. ebx was stored twice on stack.
                ; No need to clean up the 4 bytes used for function variable
                ; esp gets back to normal and it gets "forgotten" anyway!
    mov esp, ebp
    pop ebp
    iretd
