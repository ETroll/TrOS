// IDT.h
// Methods for installing and setting up the IDT

#ifndef INCLUDE_TROS_IDT_H
#define INCLUDE_TROS_IDT_H

#include <stdint.h>

#define MAX_INTERRUPTS 256

//#define IDT_DESC_BIT16 0x06
#define IDT_DESC_BIT32 0x0E
#define IDT_DESC_RING1 0x40
#define IDT_DESC_RING2 0x20
#define IDT_DESC_RING3 0x60
#define IDT_DESC_PRESENT 0x80

typedef void (*IRQ_HANDLER_FUNC)(void);

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


// Installs interrupt handler. When INT is fired, it will call this callback
// irq_handler_register in TrOS-1 - to be renamed
int idt_install_ir(uint32_t irq, uint16_t flags, uint16_t sel, IRQ_HANDLER_FUNC ir);
int idt_initialize(uint16_t codeSel, IRQ_HANDLER_FUNC default_handler);

#endif
