%ifndef __memory_asm
%define __memory_asm

bits 16

struc MemoryMapEntry
    .baseAddress resq 1
    .length      resq 1
    .type        resd 1
    .acpi_null   resd 1
endstruc



; Returns memory size for >64M configuations
; return:   ax - KB between 1MB and 16MB
;           bx - Number of 64K blocks above 16MB
; On error: bx - 0
;           ax - -1
BIOS_MEMORY_GET_64:
    push ecx
    push edx
    xor ecx, ecx
    xor edx, edx
    mov ax, 0xe801
    int 0x15
    jc .error

    cmp ah, 0x86        ;unsupported function
    je .error

    cmp ah, 0x80        ;invalid command
    je .error
    jcxz .use_ax        ;bios may have stored it in ax,bx or cx,dx. test if cx is 0

    mov ax, cx          ;its not, so it should contain mem size; store it
    mov bx, dx

    .use_ax:
        pop edx         ;mem size is in ax and bx already, return it
        pop ecx
        ret

    .error:
        mov ax, -1
        mov bx, 0
        pop edx
        pop ecx
        ret

; Creates and stores a memory map structure at given address
;   ES:DI   Buffer pointer (Where to store the map, eg the sruct)
; return:   bp - number of entries...
BIOS_MEMORY_CREATE_MAP:
    pushad
    xor ebx, ebx
    xor bp, bp
    mov edx, 'PAMS'         ; 'SMAP', remember.. little endian..
    mov eax, 0xe820
    mov ecx, 24
    int 0x15
    jc .error

    cmp eax, 'PAMS'
    jne .error

    test ebx, ebx
    je .error
    jmp	.start

    .next_entry:
        mov edx, 'PAMS'
        mov ecx, 24
        mov eax, 0xe820
        int 0x15

    .start:
        jcxz .skip_entry

    .notext:
        mov ecx, [es:di + MemoryMapEntry.length]
        test ecx, ecx
        jne short .good_entry

        mov ecx, [es:di + MemoryMapEntry.length + 4]
        jecxz .skip_entry

    .good_entry:
        inc bp
        add di, 24

    .skip_entry:
        cmp ebx, 0
        jne .next_entry
        jmp .done

    .error:
        stc

    .done:
        popad
        ret

%endif
