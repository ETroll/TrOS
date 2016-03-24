// irq.h
// Main handling of IRQs both software and hardware

#ifndef INCLUDE_TROS_IRQ_H
#define INCLUDE_TROS_IRQ_H

#include <stdint.h>

#define MAX_IRQ 256
#define IRQ_BASE 32

typedef struct registers
{
    uint32_t ds; // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    uint32_t irq_no, err_code; // Interrupt number and error code (if applicable)
    uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} __attribute__((packed))  cpu_registers_t;

typedef void (*irq_handler)(cpu_registers_t*);

void irq_initialize();
void irq_eoi(unsigned int irq);
void irq_default_handler();
int irq_register_handler(unsigned int irq, irq_handler handler);
void irq_remove_handler(unsigned int irq);

#endif
