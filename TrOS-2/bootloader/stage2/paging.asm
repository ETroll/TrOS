; Minimalistic paging for the bootloader
;

%ifndef __paging_asm
%define __paging_asm

bits 32

; page directory table
%define PAGE_DIR		0x9C000
; 0th page table. Address must be 4KB aligned
%define PAGE_TABLE_0	0x9D000
; 768th page table. Address must be 4KB aligned
%define PAGE_TABLE_768	0x9E000
; each page table has 1024 entries
%define PAGE_TABLE_ENTRIES 1024

%define PRIV	0x3


PAGING_MINIMAL_ENABLE:
	pusha

	mov eax, PAGE_TABLE_0
	mov ebx, 0x0 | PRIV
	mov ecx, PAGE_TABLE_ENTRIES

	.loop:
		mov dword [eax], ebx
		add eax, 4
		add ebx, 4096
		loop .loop

	mov eax, PAGE_TABLE_0 | PRIV
	mov dword [PAGE_DIR], eax

	mov eax, PAGE_TABLE_768 | PRIV
	mov dword [PAGE_DIR+(768*4)], eax

	mov eax, PAGE_DIR
	mov cr3, eax

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	mov eax, PAGE_TABLE_768
	mov ebx, 0x100000 | PRIV
	mov ecx, PAGE_TABLE_ENTRIES

	.loop2:
		mov dword [eax], ebx
		add eax, 4
		add ebx, 4096
		loop .loop2

	popa
	ret

%endif
