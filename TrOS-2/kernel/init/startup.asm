extern kernel_main

global kernel_init
kernel_init:
	mov	ax, 0x10
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 0xFFFFF ;1M - 1 byte

	; Set up the itsy bisty tinyest stackframe before calling kernel
	mov ebp,esp
    push ebp

    call kernel_main

    ;should never return
    cli
    hlt
