// irq.h
// Main handling of IRQs both software and hardware

#ifndef INCLUDE_TROS_IRQ_H
#define INCLUDE_TROS_IRQ_H

#include <stdint.h>

typedef struct registers
{
    uint32_t ds; // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    uint32_t irq_no, err_code; // Interrupt number and error code (if applicable)
    uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} irq_registers_t;

//TODO:
//irq_handler_register(uint8_t irq, int (*handler) (irq_registers_t reg))
//irq_initialize()

void irq_default_handler();


#endif
