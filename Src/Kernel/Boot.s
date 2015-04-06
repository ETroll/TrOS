.global _start
_start:
    mov r0, #0x00
    ldr r1, =interrupt_vectors
    ldr r3, =hang

@ Loop trough all interrupt vectors and copy them to
@ 0x00000000 (Start of RAM)
copy_intrerrupt_vectors:
    ldr r2, [r1, #0x0]
    str r2, [r0, #0x0]
    add r0, r0, #0x4
    add r1, r1, #0x4
    cmp r1, r3
    bne copy_intrerrupt_vectors

    @ldr sp, =0x07FFFFFF
    ldr sp, =0x10000000
    @ldr sp, =0x100000
    bl kmain

interrupt_vectors:
    ldr pc,reset_handler
    ldr pc,undefined_handler
    ldr pc,swi_handler
    ldr pc,prefetch_handler
    ldr pc,data_handler
    ldr pc,unused_handler
    ldr pc,irq_handler
    ldr pc,fiq_handler
reset_handler:      .word errordump
undefined_handler:  .word errordump
swi_handler:        .word software_interrupt
prefetch_handler:   .word hang
data_handler:       .word hang
unused_handler:     .word errordump
irq_handler:        .word hardware_interrupt
fiq_handler:        .word hang
hang:
    b hang

.type mmio_write, %function
.globl mmio_write
mmio_write:
    str r1,[r0]
    bx lr

.type mmio_read, %function
.globl mmio_read
mmio_read:
    ldr r0,[r0]
    bx lr

.type dummy, %function
.globl dummy
dummy:
    bx lr

.globl NOP
NOP:
    nop

.type __irq_global_enable, %function
.globl __irq_global_enable
__irq_global_enable:
    mrs r0, cpsr
    bic r0, r0, #0x80
    msr cpsr_c, r0
    bx lr

.type __irq_global_disable, %function
.globl __irq_global_disable
__irq_global_disable:
    mrs r0, cpsr
    mvn r2, #0x80
    orr r2, r2, r0
    msr cpsr_c, r0
    bx lr

errordump:
	bl crashdump

