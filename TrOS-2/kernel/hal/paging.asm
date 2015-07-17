; paging.asm
; Platform dependent helper functions for paging support

global pmm_paging_enable
pmm_paging_enable:
	push ebp
	mov ebp,esp

    mov	eax, cr0
	cmp dword [ebp+8], 1
	je	.enable
	jmp .disable

    .enable:
        or eax, 0x80000000
        mov	cr0, eax
        jmp .done

    .disable:
        and eax, 0x7FFFFFFF
        mov	cr0, eax

    .done:
    	mov esp, ebp
    	pop ebp
    	ret; 4  ; A bit unsure about C calling convention regarding who
                ; pops the stack. The caller or the method beeing called

global pmm_paging_is_enabled
pmm_paging_is_enabled:
	push ebp
	mov ebp,esp

    mov	eax, cr0

	mov esp, ebp
	pop ebp
	ret

global pmm_load_PDBR
pmm_load_PDBR:
	push ebp
	mov ebp,esp

    mov	eax, [ebp+8]
	mov	cr3, eax

	mov esp, ebp
	pop ebp
	ret; 4

global pmmngr_get_PDBR
pmmngr_get_PDBR:
	push ebp
	mov ebp,esp

    mov	eax, cr3

	mov esp, ebp
	pop ebp
	ret; 4
