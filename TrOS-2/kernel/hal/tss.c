#include <tros/hal/gdt.h>
#include <tros/hal/tss.h>
#include <tros/tros.h>
#include <string.h>

static tss_entry_t TSS;

void tss_set_stack(unsigned short kernelSS, unsigned short kernelESP)
{
    TSS.ss0 = kernelSS;
    TSS.esp0 = kernelESP;
}

void tss_install(unsigned int sel, unsigned short ss0, unsigned short esp0)
{
    uint32_t base = (uint32_t) &TSS;

    //printk("TSS at %x Size: %d\n", base, sizeof(tss_entry_t));

    gdt_add_descriptor(sel, base, base + sizeof(tss_entry_t),
        GDT_DESC_ACCESS |
        GDT_DESC_EXEC_CODE |
        GDT_DESC_DPL |
        GDT_DESC_MEMORY,
        0);

    //! initialize TSS
    memset((void*)&TSS, 0, sizeof (tss_entry_t));

    //! set stack and segments
    TSS.ss0 = ss0;
    TSS.esp0 = esp0;
    TSS.cs = 0x0b;
    TSS.ss = 0x13;
    TSS.es = 0x13;
    TSS.ds = 0x13;
    TSS.fs = 0x13;
    TSS.gs = 0x13;

    tss_flush();
}
