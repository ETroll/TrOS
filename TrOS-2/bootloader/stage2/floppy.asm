; Simple 16 bit BIOS based floppy support implementation
;

%ifndef __floppy_asm
%define __floppy_asm

bits 16

bpbBytesPerSector       dw 512
bpbRootEntries   	    dw 224
bpbSectorsPerTrack      dw 18
bpbSectorsPerCluster    db 1
bpbNumberOfFATs         db 2
bpbSectorsPerFAT        dw 9
bpbReservedSectors      dw 1
bsDriveNumber           db 0
bpbHeadsPerCylinder     dw 2


datasector          dw 0x0000
cluster             dw 0x0000
absoluteSector      db 0x00
absoluteHead        db 0x00
absoluteTrack       db 0x00

;NOTE:
;       CHS - Cluster
;       LBA - Logical block address

; Convert CHS to LBA
CHS_TO_LBA:
    sub ax, 0x0002                      ; zero base cluster number
    xor cx, cx
    mov cl, byte [bpbSectorsPerCluster]
    mul cx
    add ax, word [datasector]
    ret


; Convert LBA to CHS
; AX - LBA Address to convert
LBA_TO_CHS:
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
    xor dx, dx                          ; prepare dx:ax for operation
    div word [bpbSectorsPerTrack]       ; calculate
    inc dl                              ; adjust for sector 0
    mov byte [absoluteSector], dl
    xor dx, dx                          ; prepare dx:ax for operation
    div word [bpbHeadsPerCylinder]      ; calculate
    mov byte [absoluteHead], dl
    mov byte [absoluteTrack], al
    ret


; Read sectors from the floppy drive
; cx - Number of sectors to read
; ax - Starting sector
; es:ebx - Buffer to read to
BIOS_FLOPPY_READSECTORS:
    .readstart
        mov di, 0x0005                  ; five retries for error
    .sectorloop
        push ax
        push bx
        push cx
        call LBA_TO_CHS                 ; convert starting sector to CHS
        mov ah, 0x02                    ; BIOS read sector
        mov al, 0x01                    ; read one sector
        mov ch, byte [absoluteTrack]    ; track
        mov cl, byte [absoluteSector]   ; sector
        mov dh, byte [absoluteHead]     ; head
        mov dl, byte [bsDriveNumber]    ; drive
        int 0x13                        ; invoke BIOS
        jnc .success                    ; test for read error
        xor ax, ax                      ; BIOS reset disk
        int 0x13                        ; invoke BIOS
        dec di                          ; decrement error counter
        pop cx
        pop bx
        pop ax
        jnz .sectorloop                 ; attempt to read again
        int 0x18
    .success
        pop cx
        pop bx
        pop ax
        add bx, word [bpbBytesPerSector]; queue next buffer
        inc ax                          ; queue next sector
        loop .readstart                 ; read next sector
        ret

%endif
