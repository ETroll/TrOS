org	0x100000                    ; Kernel starts at 1 MB

bits 32

jmp	kernel_main

%include "../bootloader/stage2/stdio.asm"

msg db "   TrOS dummy kernel", 0x0A, 0x0A
    db "   Weee!", 0x0A, 0
txt db "X ", 0x0A, 0

kernel_main:
	mov	ax, 0x10
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 0x90000

    call VGA_CLEAR_SCREEN
	mov ebx, msg
	call VGA_PUTS

    mov ax, 0
    call VGA_GOTO_XY
	.restart:
		mov al, 0
	.loop:
		mov bl, al
		mov bh, 10
		call VGA_SET_COLOR

		mov ebx, txt
		mov cl, al
		add cl, 0x30
		mov [ebx], byte cl
		call VGA_PUTS

		add al, 1
		cmp al, 10
		je STOP
		jmp .loop

STOP:
	cli
	hlt
