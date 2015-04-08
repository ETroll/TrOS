@
@  Context switching functionality
@
@ Change modes setting flags in CPSR_c
@ 0xDF -> System Mode / (Accesses User Mode registers)
@ 0xD3 -> Supervisor Mode
@ 0xD2 -> IRQ Mode
@ 0xD1 -> FIQ Mode

@Same as "Switch"?
.type activate, %function
.global activate
activate:
    mov ip, sp
    @Save our current kernel state
    push {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}

    ldmfd r0!, {ip,lr}
    msr SPSR, ip
    msr CPSR_c, #0xDF

    mov sp, r0
    pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
    pop {r7}
    msr CPSR_c, #0xD3

    movs pc, lr


.global software_interrupt
software_interrupt:

    msr CPSR_c, #0xDF
    push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
    mov r0, sp
    msr CPSR_c, #0xD3

    mrs ip, SPSR
    stmfd r0!, {ip,lr}

    pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
    mov sp, ip
    bx lr

.global hardware_interrupt
hardware_interrupt:
    msr CPSR_c, #0xDF

    push {r7}

    mov r7, #0x100
    push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
    mov r0, sp

    msr CPSR_c, #0xD2
    mrs ip, SPSR
    @ for some strange reason we have to subtract 4 when fetching the lr
    @ in IRQ mode
    sub lr, lr, #0x4
    stmfd r0!, {ip,lr}

    msr CPSR_c, #0xD3

    @ load kernel state and move in to kernel
    pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
    mov sp, ip
    bx lr
