org	0x100000                    ; Kernel starts at 1 MB

bits 32

jmp	kernel_main

%include "stdio.asm"

msg db "TrOS dummy kernel", 0x0A, 0x0A
    db "Weee!", 0

kernel_main:
	mov	ax, 0x10
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 0x90000

	call VGA_CLEAR_SCREEN
	mov	ebx, msg
	call VGA_PUTS

	cli
	hlt
