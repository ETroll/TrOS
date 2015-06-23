org 0x500		; Remember the memory map-- 0x500 through 0x7bff is unused above the BIOS data area.
bits 16			; We are loaded at 0x500 (0x50:0)

jmp KRNLOAD_MAIN

%include "stdio.asm"
%include "gdt.asm"
%include "fat12.asm"

; where the kernel is to be loaded to in protected mode
%define IMAGE_PMODE_BASE 0x100000

; where the kernel is to be loaded to in real mode
%define IMAGE_RMODE_BASE 0x3000


ImageName   db "KERNEL  BIN"
ImageSize   db 0
LoadingMsg 	db "Stage2: Preparing to load kernel.", 0x0D, 0x0A, 0x00
FailureMsg db 0x0D, 0x0A, "*** FATAL: MISSING OR CURRUPT KERNEL.BIN. Press Any Key to Reboot", 0x0D, 0x0A, 0x0A, 0x00


;EnableA20 trough keyboard output port
BIOS_ENABLE_A20:
	cli
	pusha

    call wait_input
    mov al,0xAD
    out 0x64,al			;Disable keyboard
    call wait_input

    mov al,0xD0
    out 0x64,al			;Tell controller to read output port
    call wait_output

    in al,0x60
    push eax			;Get output port data and store it
    call wait_input

    mov al,0xD1
    out 0x64,al			;Tell controller to write output port
    call wait_input

    pop eax
    or al,2				;Set bit 1 (enable a20)
    out 0x60,al			;Write out data back to the output port

    call wait_input
    mov al,0xAE			;Enable keyboard
    out 0x64,al

	call wait_input

	popa
    sti
    ret

wait_input:
    in      al,0x64
    test    al,2
    jnz     wait_input
    ret

wait_output:
    in      al,0x64
    test    al,1
    jz      wait_output
    ret

KRNLOAD_MAIN:
	cli
	xor	ax, ax
	mov	ds, ax
	mov	es, ax
	mov	ax, 0x00
	mov	ss, ax
	mov	sp, 0xFFFF
	sti

	call	BIOS_INSTALL_GDT
	call 	BIOS_ENABLE_A20
	mov	si, LoadingMsg
	call	BIOS_PUTS

	call BIOS_FAT12_LOADROOT

	;Start loading the kernel
	mov ebx, 0
	mov bp, IMAGE_RMODE_BASE
	mov si, ImageName
	call BIOS_FAT12_LOAD_FILE
	mov dword [ImageSize], ecx

	cmp ax, 0
	je JUMP_TO_PROTECTED_MODE
	mov si, FailureMsg
	call BIOS_PUTS

	mov ah, 0
	int 0x16				; Wait for key press
	int 0x19				; Reboot
	cli
	hlt

JUMP_TO_PROTECTED_MODE:
	cli
	mov	eax, cr0			; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax

	jmp	08h:PROTECTED_MODE	; Far jump to fix CS.
							; Remember that the code selector is 0x8!


bits 32						; Woohoo! 32bit land! Finaly!
PROTECTED_MODE:
	mov	ax, GDT_DATA_DESC	; set data segments to data selector (0x10)
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 0x90000		; stack begins from 0x90000

	; Copy the image to protected mode address
	mov	eax, dword [ImageSize]
	movzx ebx, word [bpbBytesPerSector]
	mul	ebx
	mov	ebx, 4
	div	ebx
	cld
	mov esi, IMAGE_RMODE_BASE
	mov	edi, IMAGE_PMODE_BASE
	mov	ecx, eax
	rep	movsd

	; Jump to the kernel. - This assumes Kernel's entry point is at 1 MB
	jmp	GDT_CODE_DESC:IMAGE_PMODE_BASE


	cli
	hlt
