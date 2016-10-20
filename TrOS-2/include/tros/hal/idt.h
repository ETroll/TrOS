// IDT.h
// Methods for installing and setting up the IDT

#ifndef INCLUDE_TROS_IDT_H
#define INCLUDE_TROS_IDT_H

#include <stdint.h>

#define MAX_INTERRUPTS 256

typedef struct {
    uint16_t baseLo;
    uint16_t sel;
    uint8_t reserved;
    uint8_t flags;
    uint16_t baseHi;
} __attribute__((packed)) idt_descriptor;

typedef struct {
    uint16_t size;
    uint32_t base_address;
} __attribute__((packed)) idtr;


extern void idt_load(uint32_t addr);

// Installs interrupt handler. When INT is fired, it will call this callback
// irq_handler_register in TrOS-1 - to be renamed
void idt_install_ir(uint8_t irq, uint32_t base, uint16_t sel, uint8_t flags);
void idt_initialize();

#endif
