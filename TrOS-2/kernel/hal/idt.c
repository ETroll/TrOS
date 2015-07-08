#include <TrOS/hal/idt.h>
#include <string.h>

#include <TrOS/TrOS.h>

static idt_descriptor __idt_descriptors[MAX_INTERRUPTS];
static idtr __idtr;


int idt_install_ir(uint32_t irq, uint16_t flags, uint16_t sel, IRQ_HANDLER_FUNC ir)
{
    if (ir)
    {
        memset((void*)&__idt_descriptors[irq],
            0,
            sizeof(idt_descriptor));

        uint32_t uiBase = (uint32_t)&(*ir);

        __idt_descriptors[irq].baseLo = (uiBase & 0xffff);
        __idt_descriptors[irq].baseHi = ((uiBase >> 16) & 0xffff);
        __idt_descriptors[irq].reserved = 0;
        __idt_descriptors[irq].flags = flags;
        __idt_descriptors[irq].sel = sel;

        //printk("Installed: IRQ%d to %x desc size: %d\n", irq, &(*ir), sizeof(idt_descriptor));

        return	1;
    }
    else
    {
        return 0;
    }
}
int idt_initialize(uint16_t codeSel, IRQ_HANDLER_FUNC default_handler)
{
    __idtr.size = (sizeof(idt_descriptor) * MAX_INTERRUPTS) - 1;
    __idtr.base_address = (unsigned int)&__idt_descriptors[0];

    printk("IDTR %x Base: %x, Size: %d/%x\n", &__idtr, __idtr.base_address, __idtr.size, __idtr.size);
    printk("Default handler: %x\n", default_handler);

    for (int i = 0; i < MAX_INTERRUPTS; i++)
    {
        idt_install_ir(i,
            IDT_DESC_PRESENT | IDT_DESC_BIT32,
            codeSel,
            default_handler);
    }

    __asm("lidt (%0)" : : "m"(__idtr));
    return 1;
}
