#include <tros/hal/gdt.h>
#include <tros/hal/tss.h>
#include <string.h>

static tss_entry_t TSS;

void tss_set_stack(unsigned short kernelSS, unsigned short kernelESP)
{
    TSS.ss0 = kernelSS;
    TSS.esp0 = kernelESP;
}

void tss_install(unsigned int sel, unsigned short kernelSS, unsigned short kernelESP)
{
    uint32_t base = (uint32_t) &TSS;

    gdt_add_descriptor(sel, base, base + sizeof(tss_entry_t),
        GDT_DESC_ACCESS |
        GDT_DESC_EXEC_CODE |
        GDT_DESC_DPL |
        GDT_DESC_MEMORY,
        0);

    //! initialize TSS
    memset((void*)&TSS, 0, sizeof (tss_entry_t));

    //! set stack and segments
    TSS.ss0 = kernelSS;
    TSS.esp0 = kernelESP;
    TSS.cs = 0x0b;
    TSS.ss = 0x13;
    TSS.es = 0x13;
    TSS.ds = 0x13;
    TSS.fs = 0x13;
    TSS.gs = 0x13;
}
