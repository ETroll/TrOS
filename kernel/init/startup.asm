; Startup.asm
; Set up a stackframe and push boot paramaters to kernel_main
; Handle a kernel return if it should occur

;Create a minimal 16K section for kernel stack
section .kernel_stack, nobits
align 4
stack_bottom:
;times 16384 resb 0
resb 16384
stack_top:

section .text
extern kernel_main

global kernel_init
kernel_init:
	mov dx, 0x10
	mov ds, dx
	mov ss, dx
	mov es, dx
	mov esp, stack_top;0x90000 ; reset stack back to where we started and overwrite

	; Set up the itsy bisty tinyest "stackframe" before calling kernel
	mov ebp,esp
    push ebp

	push stack_top+8
	push eax ; multiboot "magic"
	push ebx ; multiboot struct pointer

	call kernel_main

    ;should never return
    cli
    hlt
