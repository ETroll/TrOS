org 0x500		; Remember the memory map-- 0x500 through 0x7bff is unused above the BIOS data area.
bits 16			; We are loaded at 0x500 (0x50:0)

jmp KRNLOAD_MAIN

%include "stdio.asm"
%include "gdt.asm"
%include "fat12.asm"
%include "elf32.asm"
%include "multiboot.asm"
%include "memory.asm"
%include "paging.asm"

bits 16

; where the kernel is to be loaded to in real mode
%define IMAGE_RMODE_BASE 0x3000

multiboot_data:
istruc multiboot_info
	at multiboot_info.flags,				dd 0
	at multiboot_info.memoryLo,				dd 0
	at multiboot_info.memoryHi,				dd 0
	at multiboot_info.bootDevice,			dd 0
	at multiboot_info.cmdLine,				dd 0
	at multiboot_info.mods_count,			dd 0
	at multiboot_info.mods_addr,			dd 0
	at multiboot_info.syms0,				dd 0
	at multiboot_info.syms1,				dd 0
	at multiboot_info.syms2,				dd 0
	at multiboot_info.mmap_length,			dd 0
	at multiboot_info.mmap_addr,			dd 0
	at multiboot_info.drives_length,		dd 0
	at multiboot_info.drives_addr,			dd 0
	at multiboot_info.config_table,			dd 0
	at multiboot_info.bootloader_name,		dd 0
	at multiboot_info.apm_table,			dd 0
	at multiboot_info.vbe_control_info,		dd 0
	at multiboot_info.vbe_mode_info,		dw 0
	at multiboot_info.vbe_interface_seg,	dw 0
	at multiboot_info.vbe_interface_off,	dw 0
	at multiboot_info.vbe_interface_len,	dw 0
iend

ImageName   db "KERNEL  ELF"
ImageSize   db 0
LoadingMsg 	db "Stage2: Preparing to load kernel.", 0x0D, 0x0A, 0x00
FailureMsg 	db 0x0D, 0x0A, "*** FATAL: MISSING OR CURRUPT KERNEL. Press Any Key to Reboot", 0x0D, 0x0A, 0x0A, 0x00
BadHeader 	db "*** FATAL: KERNEL.ELF is not a valid ELF32 file", 0x0A, 0x00
ElfHeader	db "Sec HdrAdr | Sec VMaddr | ELF Offset | Size bytes | Type       | Action", 0x0A, 0x00
Separator	db " | ", 0x00
StrMove		db "Move", 0x00
StrClear	db "Clear", 0x00

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
    in al,0x64
    test al,2
    jnz wait_input
    ret

wait_output:
    in al,0x64
    test al,1
    jz wait_output
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

	mov [multiboot_data+multiboot_info.bootDevice], dl

	call	BIOS_INSTALL_GDT
	call 	BIOS_ENABLE_A20
	mov	si, LoadingMsg
	call	BIOS_PUTS

	xor	eax, eax
	xor	ebx, ebx
	call BIOS_MEMORY_GET_64

	mov word [multiboot_data+multiboot_info.memoryHi], bx
	mov word [multiboot_data+multiboot_info.memoryLo], ax

	mov eax, 0x0
	mov ds, ax
	mov di, 0x1000
	call BIOS_MEMORY_CREATE_MAP

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


;------------------ 32 BIT -----------------------

bits 32						; Woohoo! 32bit land! Finaly!

ElfTempAddr				dd 0
Elf_SectionHeaderAddr	dd	0
Elf_NumSectionHeaders	db	0
Elf_SectionHeaderSize	db	0

; NOTE to self:
;	word -> 2 bytes
;	dword -> 4 bytes
; ---- ---- ---- ----
;	QEMU: info registers
;	QEMU: xp /16xw 0x100000 (prints 16 words staring at 0x100000)
PROTECTED_MODE:
	mov	ax, GDT_DATA_DESC	; set data segments to data selector (0x10)
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 0x90000		; stack begins from 0x90000

	; Lets clear the screen
	mov bl, 15
	mov bh, 0
	call VGA_SET_COLOR
	call VGA_CLEAR_SCREEN

	call PAGING_MINIMAL_ENABLE

	; Create space for som local variables
	;mov ebp, esp
	;sub esp, 8
	;mov [ebp-4], dword [IMAGE_RMODE_BASE + Elf32_Ehdr.e_shoff]

	mov ebx, dword [IMAGE_RMODE_BASE]
	cmp ebx, 0x464C457F;
	jne near BAD_KERNEL

	mov eax, dword [IMAGE_RMODE_BASE + Elf32_Ehdr.e_shoff]
	add eax, IMAGE_RMODE_BASE
	mov dword [Elf_SectionHeaderAddr], eax

	mov al, byte [IMAGE_RMODE_BASE + Elf32_Ehdr.e_shnum]
	mov byte [Elf_NumSectionHeaders], al

	mov al, byte [IMAGE_RMODE_BASE + Elf32_Ehdr.e_shentsize]
	mov byte [Elf_SectionHeaderSize], al

	; Loop trough all section headers and copy the data
	mov	ebx, ElfHeader
	call VGA_PUTS

	mov cl, 0
	.sectionloop:
		cmp cl, byte [Elf_NumSectionHeaders]
		je near EXECUTE_KERNEL

		push dword [Elf_SectionHeaderAddr]
		call VGA_PUT_HEX

		; Get VMem address. (Actually physical in this case)
		mov edx, dword [Elf_SectionHeaderAddr];
		cmp dword [edx + Elf32_Shdr.sh_addr], 0
		je near .next

		mov	ebx, Separator
		call VGA_PUTS

		push dword [edx + Elf32_Shdr.sh_addr]
		call VGA_PUT_HEX

		mov	ebx, Separator
		call VGA_PUTS

		; Get current offset from start of ELF
		push dword [edx + Elf32_Shdr.sh_offset]
		call VGA_PUT_HEX

		mov	ebx, Separator
		call VGA_PUTS

		; Get the size in bytes
		push dword [edx + Elf32_Shdr.sh_size]
		call VGA_PUT_HEX

		mov	ebx, Separator
		call VGA_PUTS

		; Get the type
		mov ebx, dword [edx + Elf32_Shdr.sh_type]
		mov eax, ebx;
		push ebx
		call VGA_PUT_HEX

		mov	ebx, Separator
		call VGA_PUTS

		push dword [edx + Elf32_Shdr.sh_size]		; size
		push dword [edx + Elf32_Shdr.sh_addr]		; to / VM Addr

		cmp eax, 1
		je .movebytes

		call MEM_CLEAR_BYTES
		mov	ebx, StrClear
		call VGA_PUTS
		jmp .next

	.movebytes:
		mov ebx, dword [edx + Elf32_Shdr.sh_offset]
		add ebx, IMAGE_RMODE_BASE
		push ebx 									; from
		call MEM_MOVE_BYTES
		mov	ebx, StrMove
		call VGA_PUTS

	.next:
		inc cl
		xor eax, eax
		mov al, byte [Elf_SectionHeaderSize]
		add dword [Elf_SectionHeaderAddr], eax

		mov bl, 0x0A
		call VGA_PUTCH
		jmp .sectionloop

EXECUTE_KERNEL:
	mov eax, dword [IMAGE_RMODE_BASE + Elf32_Ehdr.e_entry]
	mov ebp, eax
	cli

    xchg bx, bx ; If using BOCHS, lets stop up a bit

	; Execute Kernel
	mov	eax, 0x2badb002				; multiboot magic. Needs to be in eax
	mov	ebx, dword multiboot_data	; Multiboot struct. Needs to be in ebx

	call ebp
	cli
	hlt

BAD_KERNEL:
	mov	ebx, BadHeader
	call VGA_PUTS
	cli
	hlt
