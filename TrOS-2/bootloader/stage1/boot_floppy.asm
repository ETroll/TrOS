;TrOS Stage1 Bootloader

;org 0x7c00
bits 16

org	0

start:
    jmp ldrmain

; Remember! We are in 16 bit country now, that means that:
;   db = 1 byte
;   dw = 2 byte
;   dd = 4 byte
bpbOEM:                 db "TrOS    "
bpbBytesPerSector:      dw 512
bpbSectorsPerCluster:   db 1
bpbReservedSectors:     dw 1
bpbNumberOfFATs:        db 2
bpbRootEntries:         dw 224
bpbTotalSectors:        dw 2880 ; Just a small "lie" for now..
bpbMedia:               db 0xF8
bpbSectorsPerFAT:       dw 9
bpbSectorsPerTrack:     dw 18
bpbHeadsPerCylinder:    dw 2
bpbHiddenSectors:       dd 0
bpbTotalSectorsBig:     dd 0
bsDriveNumber:          db 0
bsUnused:               db 0
bsExtBootSignature:     db 0x29
bsSerialNumber:         dd 0xaac0ffee
bsVolumeLabel:          db "TROS       "
bsFileSystem:           db "FAT12   "


;
; Generic print method
; SI = Pointer to input string
print:
    lodsb               ; Loads a byte, word, or doubleword from the source
                        ; operand into the AL, AX, or EAX register.
    or al, al           ; The result of the Or yielded a 0 result
    jz printDone        ; Jump if zero
    mov ah,	0x0e        ; Command code of sorts for BIOS write char
    int 0x10
    jmp print
printDone:
    ret

;
; Reads a series of sectors
; CX = Number of sectors to read
; AX = Starting sector
; ES:BX = Buffer to read to

readSectors:
    .MAIN:
        mov di, 0x0005                  ; five retries for error
    .SECTORLOOP:
        push ax
        push bx
        push cx
        call LBACHS                     ; Convert starting sector to CHS
        mov ah, 0x02                    ; Read disk sector (int 0x13 function 2)
        mov al, 0x01                    ; Read a total of 1 sector
        mov ch, BYTE [absoluteTrack]    ; track
        mov cl, BYTE [absoluteSector]   ; sector
        mov dh, BYTE [absoluteHead]     ; head
        mov dl, BYTE [bsDriveNumber]    ; Drive 0 is the floppy disk
        int 0x13                        ; Call BIOS disk access interrupt / "syscall"
        jnc .SUCCESS                    ; Jump to SUCCESS if carry not set.
                                        ; (0x13 sets carry if it fails)
        xor ax, ax                      ; Reset floppy disk function (int 0x13 function 0)
        int 0x13                        ; Call BIOS disk access interrupt / "syscall"
        dec di                          ; Decrement the error counter
        pop cx
        pop bx
        pop ax
        jnz .SECTORLOOP                 ; Attempt to read again
        int 0x18
    .SUCCESS:
        mov si, msgProgress
        call print
        pop cx
        pop bx
        pop ax
        add bx, WORD [bpbBytesPerSector]    ; queue next buffer
        inc ax                              ; queue next sector
        loop .MAIN                          ; read next sector
        ret


;************************************************;
; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster
;************************************************;

ClusterLBA:
  sub     ax, 0x0002                          ; zero base cluster number
  xor     cx, cx
  mov     cl, BYTE [bpbSectorsPerCluster]     ; convert byte to word
  mul     cx
  add     ax, WORD [datasector]               ; base data sector
  ret

;************************************************;
; Convert LBA to CHS
; AX=>LBA Address to convert
;
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
;
;************************************************;

LBACHS:
  xor     dx, dx                              ; prepare dx:ax for operation
  div     WORD [bpbSectorsPerTrack]           ; calculate
  inc     dl                                  ; adjust for sector 0
  mov     BYTE [absoluteSector], dl
  xor     dx, dx                              ; prepare dx:ax for operation
  div     WORD [bpbHeadsPerCylinder]          ; calculate
  mov     BYTE [absoluteHead], dl
  mov     BYTE [absoluteTrack], al
  ret

;
; Main bootloader method. Called by the main entry
ldrmain:
    ; Set up stack and such. Remember Segment:Offset in 16 bit mode
    ; We are located at 07C0:0000, that translats to 0x7C00 physical
    cli
    mov ax, 0x07C0      ; Load our segment into the segment registers
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ax, 0x0000      ; Set up 64K stack in segment 0x0000; Phys: 0xFFFF
    mov ss, ax          ; Bit risky, we could end up overriding the IVT if the
    mov sp, 0xFFFF      ; stack grows to big.. IVT: (Phys:) 0x0000 -> 0x0400
    sti

    mov [bootdevice], dl

loadRootDirectory:
    mov si, msgSearch
    call print
    xor cx, cx
    xor dx, dx
    mov ax, 0x0020                      ; 32 byte per directory entry
    mul WORD [bpbRootEntries]           ; Multiply it by the number of entries
    div WORD [bpbBytesPerSector]        ; Divide it by the size of a sector,
    xchg ax, cx                         ; And we now have the size for the root dir.

    mov al, BYTE [bpbNumberOfFATs]      ; Load number of FATs we have
    mul WORD [bpbSectorsPerFAT]         ; Multiply that with the number of sectors per FAT
    add ax, WORD [bpbReservedSectors]   ; Adjust for the bootsector
    mov WORD [datasector], ax           ; And now we have the start location for root dir
    add WORD [datasector], cx           ; Add the size of the root dir and we
                                        ; now have the loc of data region with files and dirs
    mov bx, 0x0200                      ; Copy root dir after this bootloader
    call readSectors

    ; We now have the root dir at 0x0200, lets search trough it.
    mov cx, WORD [bpbRootEntries]   ; Load in the amount of possible entries
    mov di, 0x0200                  ; First entry is at the start
    ._loop:
        push cx
        mov cx, 0x000B              ; 11 Bytes for file name size
        mov si, ImageName           ; Ptr to the file name to find
        push di                     ; Push di since cmpsb can modify di
        repe cmpsb                  ; StrCmp si with di
        pop di                      ; Get back the di value in case it was modified
        je loadFAT                  ; If equal, lets load the FAT
        pop cx
        add di, 0x0020              ; Add 32 bytes to point to next entry
        loop ._loop                 ; Loop decrements cx as it uses as a loop counter
        jmp notFound                ; If loop are complete we did not find

loadFAT:
    ; save starting cluster of boot image
    mov dx, WORD [di + 0x001A]
    mov WORD [cluster], dx              ; file's first cluster

    ; compute size of FAT and store in "cx"
    xor ax, ax
    mov al, BYTE [bpbNumberOfFATs]      ; number of FATs
    mul WORD [bpbSectorsPerFAT]         ; sectors used by FATs
    mov cx, ax

    ; compute location of FAT and store in "ax"
    mov ax, WORD [bpbReservedSectors]   ; adjust for bootsector

    ; read FAT into memory (7C00:0200)
    mov bx, 0x0200                      ; copy FAT above bootcode
    call readSectors

    mov     ax, 0x0050
    mov     es, ax                              ; destination for image
    mov     bx, 0x0000                          ; destination for image
    push    bx

loadImage:

    mov ax, WORD [cluster]                  ; cluster to read
    pop bx                                  ; buffer to read into
    call ClusterLBA                          ; convert cluster to LBA
    xor cx, cx
    mov cl, BYTE [bpbSectorsPerCluster]     ; sectors to read
    call readSectors
    push bx

    ; compute next cluster

    mov ax, WORD [cluster]                  ; identify current cluster
    mov cx, ax                              ; copy current cluster
    mov dx, ax                              ; copy current cluster
    shr dx, 0x0001                          ; divide by two
    add cx, dx                              ; sum for (3/2)
    mov bx, 0x0200                          ; location of FAT in memory
    add bx, cx                              ; index into FAT
    mov dx, WORD [bx]                       ; read two bytes from FAT
    test ax, 0x0001
    jnz .ODD_CLUSTER

    .EVEN_CLUSTER:
        and dx, 0000111111111111b               ; take low twelve bits
        jmp .DONE

    .ODD_CLUSTER:
        shr dx, 0x0004                          ; take high twelve bits
    .DONE:
        mov WORD [cluster], dx                  ; store new cluster
        cmp dx, 0x0FF0                          ; test for end of file
        jb loadImage

DONE:
    mov si, msgCRLF
    call print
    mov dl, [bootdevice]
    push WORD 0x0050
    push WORD 0x0000
    retf


notFound:
    mov si, msgFailure
    call print
    mov ah, 0x00
    int 0x16            ; Wait for key press
    int 0x19            ; Reboot / Warm boot

;Variables
absoluteSector  db 0x00
absoluteHead    db 0x00
absoluteTrack   db 0x00

bootdevice      db 0
datasector      dw 0x0000
cluster         dw 0x0000

;Variables / Strings
ImageName       db "KRNLDR  BIN"
msgProgress     db ".", 0x00
msgSearch       db 0x0D, 0x0A, "Searching", 0x00
msgFailure      db 0x0D, 0x0A, "ERROR: Press Any Key to Reboot", 0x0D, 0x0A, 0x00
msgCRLF         db 0x0D, 0x0A, 0x00

times 510 - ($-$$) db 0
dw 0xAA55 ; Boot Signature
