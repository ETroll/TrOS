; Simple VGA implementation for 16 and 32 bits
;

%ifndef __stdio_asm
%define __stdio_asm

; Prints a null terminated string for 16 bit mode (Real mode)
; DS=>SI: 0 terminated string

bits 16
BIOS_PUTS:
	pusha					; save registers
	.Loop1:
		lodsb				; load next byte from string from SI to AL
		or	al, al			; Does AL=0?
		jz	.puts16_done	; Yep, null terminator found-bail out
		mov	ah, 0eh			; Nope-Print the character
		int	10h				; invoke BIOS
		jmp	.Loop1			; Repeat until null terminator found
	.puts16_done:
		popa				; restore registers
		ret					; we are done, so return


bits 32
%define VIDMEM		0xB8000
%define COLS		80
%define LINES		25

;Current cursor X/Y pos
_currentX db 0
_currentY db 0
_char_attrib db 0x1F;

; Moves the hardware cursor to position given in parameters:
; bh - Y pos
; bl - X pos
VGA_MOVE_CURSOR:
	pusha
	xor eax, eax
	mov ecx, COLS
	mov al, bh
	mul ecx						; Multiply Y (al) with COLS (ecx) into al
	add al, bl
	mov ebx, eax

; 0x3D4 - CRT Controller Index Register - Data to write
; 0x3D5 - CRT Controller Data Register - Type of data to write
	mov al, 0x0F				; 0x0F = Cursor Location Low
	mov dx, 0x3D4
	out dx, al

	mov al, bl
	mov dx, 0x3D5
	out dx, al					; Set the cursor low byte

	xor eax, eax
	mov al, 0x0E				; 0x0E = Cursor Location High
	mov dx, 0x3D4
	out dx, al

	mov al, bh
	mov dx, 0x3D5
	out dx, al

	popa
	ret

; Prints a character to the VGA memory
; BL => char to print
VGA_PUTCH:
	pusha
	mov edi, VIDMEM
	xor eax, eax

	mov ecx, COLS*2				; 2 bytes per character in Mode7
	mov al, byte [_currentY]
	mul ecx						; Multiply ecx with al
	push eax					; Push the result of multiplication

	mov al, byte [_currentX]
	mov cl, 2
	mul cl
	pop ecx						; Pop result from previous mul into ecx
	add eax, ecx				; Add the X-offset

	xor ecx, ecx
	add edi, eax				; Add the calculated offset to the base addr

	cmp bl, 0x0A
	je .newline

	mov dl, bl
	mov dh, [_char_attrib]
	mov word [edi], dx			; Move the data stored in dx to memory pointed
	inc byte [_currentX]		; to by edi
	jmp .done

	.newline:
		mov byte [_currentX], 0		; Reset X pos
		inc byte [_currentY]

	.done:
		popa
		ret

; Prints a 32bit word to VGA memory
; Takes 1 argument on the stack
VGA_PUT_HEX:
	push ebp
    mov ebp,esp
    ;sub esp, 4 ; local variable of 4 bytes (accessable from [ebp-4])
	pusha
	mov bl, 0x30		; '0'
	call VGA_PUTCH
	mov bl, 0x78		; 'x'
	call VGA_PUTCH

    ;mov eax,[ebp+8] ; our parameter
	mov cl, 28
	.loop:
		cmp cl, 0
		jl .done

		mov ebx, [ebp+8]
		shr ebx, cl			; value in ecx used for shift op
		and ebx, 0xF		; bl now contains our letter
		cmp bl, 9
		jg .letter
		add bl, 0x30
		jmp .print

	.letter:
		add bl, 0x37

	.print:
		call VGA_PUTCH
		sub cl, 4			; go downwards 4 bits (nibble) at a time
		jmp .loop

	.done:
		popa
	    mov esp, ebp
	    pop ebp
	    ret 4 ; pop the parameter from the stack


;Prints a nullterminated string to VGA
; EBX => Start address of the string to print
VGA_PUTS:
	pusha
	push ebx
	pop edi						; copy value of ebx in to edi
	;mov edi, ebx				; copy value of ebx in to edi

	.loop:
		mov bl, byte [edi]
		cmp bl, 0
		je .done

		call VGA_PUTCH
		inc edi
		jmp .loop

	.done:
		mov bh, byte [_currentY]
		mov bl, byte [_currentX]
		call VGA_MOVE_CURSOR
		popa
		ret

; Clears the screen
VGA_CLEAR_SCREEN:
	pusha
	cld
	mov edi, VIDMEM
	mov cx, 2000
	mov ah, [_char_attrib]
	mov al, ' '
	rep stosw					; Loops over and increments EDI 2000 times
								; and inserts value of ax
	mov byte [_currentX], 0
	mov byte [_currentY], 0
	popa
	ret

; Set the VGA color via ATTRIB
; bl - Font color
; bh - Background color
VGA_SET_COLOR:
	pusha
	shl bh, 4
	and bl, 0x0F
	or bl, bh
	mov byte [_char_attrib], bl
	popa
	ret

VGA_SET_COLOR_DEFAULT:
	mov byte [_char_attrib], 0x1F
	ret

; set the _current values to valus of al (X pos) ah (Y pos)
VGA_GOTO_XY:
	pusha
	mov [_currentX], al
	mov [_currentY], ah
	popa
	ret

; Move data from one address to another
;					+8			+12			+16
; MEM_MOVE_BYTE(void* from, void* to, int numBytes)
MEM_MOVE_BYTES:
	push ebp
	mov ebp,esp
	pusha

	cld
	mov esi, [ebp+8]
	mov	edi, [ebp+12]
	mov ecx, [ebp+16]
	rep	movsb

	popa
	mov esp, ebp
	pop ebp
	ret 12

; Move data from one address to another
;					+8			+12			+16
; MEM_MOVE_WORDS(void* from, void* to, int numWords)
MEM_MOVE_WORDS:
	push ebp
	mov ebp,esp
	pusha

	cld
	mov esi, [ebp+8]
	mov	edi, [ebp+12]
	mov ecx, [ebp+16]
	rep	movsd

	popa
	mov esp, ebp
	pop ebp
	ret 12

; MEM_CLEAR_BYTES(void* addr, int length)
MEM_CLEAR_BYTES:
	push ebp
	mov ebp, esp
	pusha

	mov al, 0x00
	mov ebx, [ebp+8]
	mov ecx, [ebp+12]

	.loop:
		mov [ebx], al
		inc ebx
		dec ecx
		cmp ecx, 0
		jne .loop

	popa
	mov esp, ebp
	pop ebp
	ret 8
%endif
