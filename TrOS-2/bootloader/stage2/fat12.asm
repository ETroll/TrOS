; Simple FAT12 support implementation
;

%ifndef __fat12_asm
%define __fat12_asm

bits 16

%include "floppy.asm"

%define ROOT_OFFSET 0x2e00
%define FAT_SEG 0x2c0
%define ROOT_SEG 0x2e0

BIOS_FAT12_LOADROOT:
    pusha
    push es

    ; Compute the size of root directory and store in "cx"
    xor cx, cx
    xor dx, dx
    mov ax, 32	                    ; 32 byte directory entry
    mul word [bpbRootEntries]       ; Multiply 32 with num root directories
    div word [bpbBytesPerSector]    ; sectors used by directory
    xchg ax, cx

    ; Compute location of root directory and store in "ax"
    mov al, byte [bpbNumberOfFATs]
    mul word [bpbSectorsPerFAT]
    add ax, word [bpbReservedSectors]
    mov word [datasector], ax
    add word [datasector], cx

    push word ROOT_SEG
    pop es
    mov bx, 0
    call BIOS_FLOPPY_READSECTORS
    pop es
    popa
    ret

; Load FAT table in to location of FAT_SEG
; es:di - Root directory table
BIOS_FAT12_LOAD_FAT:
    pusha
    push es

    ; Compute size of FAT and store in "cx"
    xor ax, ax
    mov al, byte [bpbNumberOfFATs]
    mul word [bpbSectorsPerFAT]
    mov cx, ax

    ; Compute location of FAT and store in "ax"
    mov ax, word [bpbReservedSectors]

    ; Read FAT into memory (Overwrite our bootloader at 0x7c00)
    push word FAT_SEG
    pop es
    xor bx, bx
    call BIOS_FLOPPY_READSECTORS
    pop es
    popa
    ret

; Find file in root table
; ds:si - The file name
; return ax - The index number for the file in the direcgory table
;             -1 if an error
BIOS_FAT12_FIND_FILE:
    push	cx
    push	dx
    push	bx
    mov	bx, si ; copy filename for later

    ; Browse root directory for binary image
    mov cx, word [bpbRootEntries]
    mov di, ROOT_OFFSET
    cld

    .loop:
        push cx
        mov cx, 11
        mov	si, bx
        push di
        rep cmpsb
        pop di
        je .found_file
        pop cx
        add di, 32          ; queue next directory entry
        loop .loop

    .not_find_file:
        pop	bx
        pop	dx
        pop	cx
        mov	ax, -1
        ret

    .found_file:
        pop	ax              ; return value into AX contains entry of file
        pop	bx
        pop	dx
        pop	cx
        ret

; Load a file in to buffer
; es:si - The file to load
; ebx:bp - The buffer to load the file in to
; return ax - 0 if successfull
;             -1 if an error
;        cx - The number of sectors that were read
BIOS_FAT12_LOAD_FILE:
    xor	ecx, ecx
    push ecx

    .find_file:
        push bx
        push bp
        call BIOS_FAT12_FIND_FILE	    ; ES:SI contains the filename
        cmp	ax, -1
        jne	.load_image_pre
        pop	bp
        pop	bx
        pop	ecx
        mov ax, -1
        ret

    .load_image_pre:
        sub	edi, ROOT_OFFSET
        sub	eax, ROOT_OFFSET

        ; get starting cluster
        push word ROOT_SEG
        pop	es
        mov	dx, word [es:di + 0x001A]   ; DI points to file entry in
                                        ; root directory table.
        mov	word [cluster], dx	; file's first cluster
        pop	bx
        pop	es
        push bx
        push es
        call BIOS_FAT12_LOAD_FAT

    .load_image:
        mov	ax, word [cluster]
        pop	es                          ; bx:bp=es:bx
        pop	bx
        call CHS_TO_LBA
        xor	cx, cx
        mov cl, BYTE [bpbSectorsPerCluster]
        call BIOS_FLOPPY_READSECTORS
        pop	ecx
        inc	ecx
        push ecx
        push bx
        push es
        mov	ax, FAT_SEG
        mov	es, ax
        xor	bx, bx

        ; get next cluster
        mov ax, word [cluster]
        mov cx, ax
        mov dx, ax
        shr dx, 1
        add cx, dx

        mov	bx, 0
        add	bx, cx
        mov	dx, word [es:bx]
        test ax, 1
        jnz	.odd_cluster

    .even_cluster:
        and	dx, 0000111111111111b
        jmp	.done

    .odd_cluster:
        shr	dx, 0x0004

    .done:
        mov	word [cluster], dx
        cmp	dx, 0x0ff0		            ; test for end of file marker
        jb .load_image

    .success:
        pop	es
        pop	bx
        pop	ecx
        xor	ax, ax
        ret

%endif
