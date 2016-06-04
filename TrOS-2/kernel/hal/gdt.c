#include <tros/hal/gdt.h>
#include <tros/hal/tss.h>
#include <tros/tros.h>
#include <tros/klib/kstring.h>

static gdtr __gdtr;
static gdt_descriptor __gdt_descriptors[GDT_MAX_DESCRIPTORS];
//NOTE: If the array comes before the gdtr struct,
//      then the size will be wiped during data copy I think.
//      I need to look in to this in the bootloader

#define GDT_SEG_0_NULL 0
#define GDT_SEG_0_CODE 1
#define GDT_SEG_0_DATA 2
#define GDT_SEG_3_CODE 3
#define GDT_SEG_3_DATA 4

void gdt_initialize()
{
    __gdtr.size = (sizeof(gdt_descriptor) * GDT_MAX_DESCRIPTORS) - 1;
    __gdtr.base_address = (uint32_t)&__gdt_descriptors;

    //Null descriptor
    gdt_add_descriptor(GDT_SEG_0_NULL, 0, 0, 0, 0);

    //Code segment descriptor
    gdt_add_descriptor(GDT_SEG_0_CODE, 0, 0xFFFFFFFF,
        GDT_DESC_READWRITE |
        GDT_DESC_EXEC_CODE |
        GDT_DESC_CODEDATA |
        GDT_DESC_MEMORY,
        GDT_GRAND_4K |
        GDT_GRAND_32BIT |
        GDT_GRAND_LIMITHI_MASK);

    //Data segment descriptor
    gdt_add_descriptor(GDT_SEG_0_DATA, 0, 0xFFFFFFFF,
        GDT_DESC_READWRITE |
        GDT_DESC_CODEDATA |
        GDT_DESC_MEMORY,
        GDT_GRAND_4K |
        GDT_GRAND_32BIT |
        GDT_GRAND_LIMITHI_MASK);

    // User mode code segment
	gdt_add_descriptor(GDT_SEG_3_CODE, 0, 0xFFFFFFFF,
        GDT_DESC_READWRITE |
        GDT_DESC_EXEC_CODE |
        GDT_DESC_CODEDATA |
        GDT_DESC_MEMORY |
        GDT_DESC_DPL,
        GDT_GRAND_4K |
        GDT_GRAND_32BIT |
        GDT_GRAND_LIMITHI_MASK);

	// User mode data segment
	gdt_add_descriptor(GDT_SEG_3_DATA, 0, 0xFFFFFFFF,
        GDT_DESC_READWRITE |
        GDT_DESC_CODEDATA |
        GDT_DESC_MEMORY |
        GDT_DESC_DPL,
        GDT_GRAND_4K |
        GDT_GRAND_32BIT |
        GDT_GRAND_LIMITHI_MASK);

    printk("GDTR %x Base: %x, Size: %d bytes\n", &__gdtr, __gdtr.base_address, __gdtr.size);

    //TODO: Move TSS and GDT functionality together..
    tss_install(5);
    gdt_load((uint32_t)&__gdtr);
    tss_flush(5 * sizeof(gdt_descriptor));

    // NOTE:
    // https://gcc.gnu.org/onlinedocs/gcc/Simple-Constraints.html#Simple-Constraints

}
void gdt_add_descriptor(uint32_t loc, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran)
{
    // Clear out old "junk" if any
    memset((void*)&__gdt_descriptors[loc],
        0,
        sizeof(gdt_descriptor));

    __gdt_descriptors[loc].baseLo = (base & 0xffff);
    __gdt_descriptors[loc].baseMid = ((base >> 16) & 0xff);
    __gdt_descriptors[loc].baseHi = ((base >> 24) & 0xff);
    __gdt_descriptors[loc].limit = (limit & 0xffff);
    __gdt_descriptors[loc].access_flags = access;
    __gdt_descriptors[loc].granularity = ((limit >> 16) & 0x0f);
    __gdt_descriptors[loc].granularity |= gran & 0xf0;
}
