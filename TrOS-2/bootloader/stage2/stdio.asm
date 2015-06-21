

%ifndef __stdio_asm
%define __stdio_asm

; Prints a null terminated string for 16 bit mode (Real mode)
; DS=>SI: 0 terminated string

bits 16
Puts16:
	pusha					; save registers
	.Loop1:
		lodsb				; load next byte from string from SI to AL
		or	al, al			; Does AL=0?
		jz	Puts16Done		; Yep, null terminator found-bail out
		mov	ah, 0eh			; Nope-Print the character
		int	10h				; invoke BIOS
		jmp	.Loop1			; Repeat until null terminator found
Puts16Done:
	popa					; restore registers
	ret						; we are done, so return

%endif
