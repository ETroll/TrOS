@
@ TODO: Move out as a userland lib..
@

.type yield, %function
.global yield
yield:
    push {r7}
    mov r7, #0x0
    svc 0
    bx lr

.type fork, %function
.global fork
fork:
    push {r7}
    mov r7, #0x1
    svc 0
    bx lr

.type exit, %function
.global exit
exit:
    push {r7}
    mov r7, #0x2
    svc 0
    bx lr

.type getpid, %function
.global getpid
getpid:
    push {r7}
    mov r7, #0x3
    svc 0
    bx lr

.type write, %function
.global write
write:
    push {r7}
    mov r7, #0x4
    svc 0
    bx lr

.type read, %function
.global read
read:
    push {r7}
    mov r7, #0x5
    svc 0
    bx lr

.type open, %function
.global open
open:
    push {r7}
    mov r7, #0x6
    svc 0
    bx lr

.type close, %function
.global close
close:
    push {r7}
    mov r7, #0x7
    svc 0
    bx lr

 .type peek, %function
.global peek
peek:
    push {r7}
    mov r7, #0x8
    svc 0
    bx lr

.type sleep, %function
.global sleep
sleep:
    push {r7}
    mov r7, #0x9
    svc 0
    bx lr
