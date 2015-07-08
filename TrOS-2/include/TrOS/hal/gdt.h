// GDT.h
// Methods for installing and setting up the GDT

#ifndef INCLUDE_TROS_GDT_H
#define INCLUDE_TROS_GDT_H

#include <stdint.h>

#define GDT_MAX_DESCRIPTORS 5

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
