#include <tros/hal/gdt.h>
#include <tros/hal/tss.h>
#include <tros/tros.h>
#include <tros/klib/kstring.h>

static tss_entry_t TSS;

void tss_set_ring0_stack(unsigned short segment, unsigned int stackptr)
{
    TSS.ss0 = segment;
    TSS.esp0 = stackptr;
}

void tss_install(unsigned int sel)
{
    uint32_t base = (uint32_t) &TSS;

    gdt_add_descriptor(sel, base, base + sizeof(tss_entry_t),
        GDT_DESC_ACCESS |
        GDT_DESC_EXEC_CODE |
        GDT_DESC_DPL |
        GDT_DESC_MEMORY,
        0);

    memset((void*)&TSS, 0, sizeof (tss_entry_t));

    //Setting default segments. The ss0 and esp0 will be changed
    //before switching to ring3
    TSS.ss0 = 0x10;
    TSS.esp0 = 0x0;
    TSS.cs = 0x0b;
    TSS.ss = 0x13;
    TSS.es = 0x13;
    TSS.ds = 0x13;
    TSS.fs = 0x13;
    TSS.gs = 0x13;
}
