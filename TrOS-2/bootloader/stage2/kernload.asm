org 0x500		; Remember the memory map-- 0x500 through 0x7bff is unused above the BIOS data area.
bits 16			; We are loaded at 0x500 (0x50:0)

jmp main

%include "stdio.asm"
%include "gdt.asm"

LoadingMsg 	db "Stage2: Preparing to load operating system.", 0x0D, 0x0A, 0x00
A20Msg		db "Stage2: A20 enabled. Full 32 bit address bus is now available", 0x0D, 0x0A, 0x00

;EnableA20 trough keyboard output port
EnableA20:
	cli
	pusha
	    call    wait_input
	    mov     al,0xAD
	    out     0x64,al			;Disable keyboard
	    call    wait_input

	    mov     al,0xD0
	    out     0x64,al			;Tell controller to read output port
	    call    wait_output

	    in      al,0x60
	    push    eax				;Get output port data and store it
	    call    wait_input

	    mov     al,0xD1
	    out     0x64,al			;Tell controller to write output port
	    call    wait_input

	    pop     eax
	    or      al,2			;Set bit 1 (enable a20)
	    out     0x60,al			;Write out data back to the output port

	    call    wait_input
	    mov     al,0xAE			;Enable keyboard
	    out     0x64,al

	    call    wait_input
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

main:
	cli
	xor	ax, ax
	mov	ds, ax
	mov	es, ax
	mov	ax, 0x9000		; stack begins at 0x9000-0xffff
	mov	ss, ax
	mov	sp, 0xFFFF
	sti

	mov	si, LoadingMsg
	call	Puts16
	call	InstallGDT
	call 	EnableA20
	mov si, A20Msg
	call 	Puts16

	cli
	mov	eax, cr0		; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax

	jmp	08h:stage3		; far jump to fix CS. Remember that the code selector is 0x8!

bits 32					; Woohoo! 32bit land! Finaly!

stage3:
	mov		ax, 0x10	; set data segments to data selector (0x10)
	mov		ds, ax
	mov		ss, ax
	mov		es, ax
	mov		esp, 0x90000; stack begins from 0x90000

stop:
	cli
	hlt
