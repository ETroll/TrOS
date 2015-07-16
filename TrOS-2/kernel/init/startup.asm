; Startup.asm
; Set up a stackframe and push boot paramaters to kernel_main
; Handle a kernel return if it should occur

extern kernel_main

global kernel_init
kernel_init:
	mov	dx, 0x10
	mov	ds, dx
	mov	ss, dx
	mov	es, dx
	mov	esp, 0x90000 ; reset stack back to where we started and overwrite

	; Set up the itsy bisty tinyest "stackframe" before calling kernel
	mov ebp,esp
    push ebp

	push eax		; multiboot "magic"
	push ecx		; kernel size
	push ebx		; multiboot struct pointer

	call kernel_main

    ;should never return
    cli
    hlt
