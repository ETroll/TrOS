;
; Helper methods for IRQ handling

.global reload_segments
reload_segments:
    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush

    .flush:
        ret
