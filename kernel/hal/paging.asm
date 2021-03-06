; paging.asm
; Platform dependent helper functions for paging support

global paging_enable
paging_enable:
	push ebp
	mov ebp,esp

    mov eax, cr0
	cmp dword [ebp+8], 1
	je .enable
	jmp .disable

    .enable:
        or eax, 0x80000000
        mov cr0, eax
        jmp .done

    .disable:
        and eax, 0x7FFFFFFF
        mov cr0, eax

    .done:
    	mov esp, ebp
    	pop ebp
    	ret; 4  ; A bit unsure about C calling convention regarding who
                ; pops the stack. The caller or the method beeing called

global paging_is_enabled
paging_is_enabled:
	push ebp
	mov ebp,esp

    mov eax, cr0

	mov esp, ebp
	pop ebp
	ret

global paging_set_CR3
paging_set_CR3:
	push ebp
	mov ebp,esp

    mov eax, [ebp+8]
	mov cr3, eax

	mov esp, ebp
	pop ebp
	ret; 4

global paging_get_CR3
paging_get_CR3:
	push ebp
	mov ebp,esp

    mov eax, cr3

	mov esp, ebp
	pop ebp
	ret; 4

global paging_error_addr
paging_error_addr:
	push ebp
	mov ebp,esp

    mov eax, cr2

	mov esp, ebp
	pop ebp
	ret; 4

;Invalidate entry
global paging_flush_tlb_entry
paging_flush_tlb_entry:
	push ebp
	mov ebp,esp

	cli
	invlpg [ebp+8]
	sti

	mov esp, ebp
	pop ebp
	ret
