%ifndef __gdt_asm
%define __gdt_asm

bits	16
InstallGDT:
	cli
	pusha
	lgdt [toc]			; load GDT into GDTR
	sti
	popa
	ret

gdt_data:
	dd 0 				; null descriptor
	dd 0
; gdt code:				; code descriptor
	dw 0xFFFF 			; limit low
	dw 0 				; base low
	db 0 				; base middle
	db 10011010b 		; access
	db 11001111b 		; granularity
	db 0 				; base high
; gdt data:				; data descriptor
	dw 0xFFFF 			; limit low (Same as code)
	dw 0 				; base low
	db 0 				; base middle
	db 10010010b		; access
	db 11001111b		; granularity
	db 0				; base high
end_of_gdt:
toc:
	dw end_of_gdt - gdt_data - 1
	dd gdt_data


%endif
