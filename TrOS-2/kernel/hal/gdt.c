#include <TrOS/hal/gdt.h>
#include <string.h>

static gdt_descriptor __gdt_descriptors[GDT_MAX_DESCRIPTORS];
static int __gdt_counter = 0;
static gdtr __gdtr;

void gdt_initialize()
{
    __gdtr.size = (sizeof(gdt_descriptor) * GDT_MAX_DESCRIPTORS) - 1;
    __gdtr.base_address = (unsigned int)&__gdt_descriptors[0];

    //Null descriptor
    gdt_add_descriptor(0, 0, 0, 0);

    //Code segment descriptor
    gdt_add_descriptor (0, 0xFFFFFFFF,
        GDT_DESC_READWRITE |
        GDT_DESC_EXEC_CODE |
        GDT_DESC_CODEDATA |
        GDT_DESC_MEMORY,
        GDT_GRAND_4K |
        GDT_GRAND_32BIT |
        GDT_GRAND_LIMITHI_MASK);

    //Data segment descriptor
    gdt_add_descriptor (0, 0xFFFFFFFF,
        GDT_DESC_READWRITE |
        GDT_DESC_CODEDATA |
        GDT_DESC_MEMORY,
        GDT_GRAND_4K |
        GDT_GRAND_32BIT |
        GDT_GRAND_LIMITHI_MASK);

    __asm("lgdt (%0)" : : "m"(__gdtr));
    // NOTE:
    // https://gcc.gnu.org/onlinedocs/gcc/Simple-Constraints.html#Simple-Constraints

}
void gdt_add_descriptor(uint64_t base, uint64_t limit, uint8_t access, uint8_t grand)
{
    memset((void*)&__gdt_descriptors[__gdt_counter],
        0,
        sizeof(gdt_descriptor));

    __gdt_descriptors[__gdt_counter].baseLo	= (base & 0xffff);
    __gdt_descriptors[__gdt_counter].baseMid = ((base >> 16) & 0xff);
    __gdt_descriptors[__gdt_counter].baseHi	= ((base >> 24) & 0xff);
    __gdt_descriptors[__gdt_counter].limit	= (limit & 0xffff);
    __gdt_descriptors[__gdt_counter].flags = access;
    __gdt_descriptors[__gdt_counter].grand = ((limit >> 16) & 0x0f);
    __gdt_descriptors[__gdt_counter].grand |= grand & 0xf0;
    __gdt_counter++;
}
