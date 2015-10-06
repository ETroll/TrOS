// GDT.h
// Methods for installing and setting up the GDT

#ifndef INCLUDE_TROS_GDT_H
#define INCLUDE_TROS_GDT_H

#include <stdint.h>

#define GDT_MAX_DESCRIPTORS 5

// GDT descriptor flags
// Set access bit
#define GDT_DESC_ACCESS 0x01
// Descriptor is readable and writable. default: read only
#define GDT_DESC_READWRITE 0x02
// Set expansion direction bit
#define GDT_DESC_EXPANSION 0x04
// Executable code segment. Default: data segment
#define GDT_DESC_EXEC_CODE 0x08
// Set code or data descriptor. Default: system defined descriptor
#define GDT_DESC_CODEDATA 0x10
// Set dpl bits
#define GDT_DESC_DPL 0x60
// Set "in memory" bit
#define GDT_DESC_MEMORY 0x80

// GDT descriptor grandularity bit flags:
// Masks out limitHi (High 4 bits of limit)
#define GDT_GRAND_LIMITHI_MASK 0x0F
// Set os defined bit
#define GDT_GRAND_OS 0x10
// Enable 32 bit
#define GDT_GRAND_32BIT 0x40
// 4k grandularity. default: none
#define GDT_GRAND_4K 0x80

typedef struct
{
    uint16_t limit;
    uint16_t baseLo;
    uint8_t baseMid;
    uint8_t access_flags;
    uint8_t granularity;
    uint8_t baseHi;
} __attribute__((packed)) gdt_descriptor;

typedef struct{
    uint16_t size;
    uint32_t base_address;
} __attribute__((packed)) gdtr;


extern void gdt_load(uint32_t addr);

void gdt_initialize();
void gdt_add_descriptor(uint32_t loc, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran);


#endif
