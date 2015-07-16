#include <tros/hal/idt.h>
#include <tros/hal/io.h>
#include <string.h>

#include <tros/tros.h>

static idtr __idtr;
static idt_descriptor __idt_descriptors[MAX_INTERRUPTS];

#define IDT_DESC_BIT16 0x06
#define IDT_DESC_BIT32 0x0E
#define IDT_DESC_RING1 0x40
#define IDT_DESC_RING2 0x20
#define IDT_DESC_RING3 0x60
#define IDT_DESC_PRESENT 0x80

extern void irq_fallback();

extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void irq0 ();
extern void irq1 ();
extern void irq2 ();
extern void irq3 ();
extern void irq4 ();
extern void irq5 ();
extern void irq6 ();
extern void irq7 ();
extern void irq8 ();
extern void irq9 ();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void idt_install_ir(uint8_t irq, uint32_t base, uint16_t sel, uint8_t flags)
{
    __idt_descriptors[irq].baseLo = base & 0xFFFF;
    __idt_descriptors[irq].baseHi = (base >> 16) & 0xFFFF;

    __idt_descriptors[irq].sel     = sel;
    __idt_descriptors[irq].reserved = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    __idt_descriptors[irq].flags   = flags /* | 0x60 */;
}

void idt_initialize()
{
    __idtr.size = (sizeof(idt_descriptor) * MAX_INTERRUPTS) - 1;
    __idtr.base_address = (unsigned int)&__idt_descriptors[0];

    memset(&__idt_descriptors,
        0,
        sizeof(idt_descriptor) * MAX_INTERRUPTS);

    printk("IDTR %x Base: %x, Size: %d/%x\n", &__idtr, __idtr.base_address, __idtr.size, __idtr.size);

    idt_install_ir( 0, (uint32_t)isr0 , 0x08, 0x8E);
    idt_install_ir( 1, (uint32_t)isr1 , 0x08, 0x8E);
    idt_install_ir( 2, (uint32_t)isr2 , 0x08, 0x8E);
    idt_install_ir( 3, (uint32_t)isr3 , 0x08, 0x8E);
    idt_install_ir( 4, (uint32_t)isr4 , 0x08, 0x8E);
    idt_install_ir( 5, (uint32_t)isr5 , 0x08, 0x8E);
    idt_install_ir( 6, (uint32_t)isr6 , 0x08, 0x8E);
    idt_install_ir( 7, (uint32_t)isr7 , 0x08, 0x8E);
    idt_install_ir( 8, (uint32_t)isr8 , 0x08, 0x8E);
    idt_install_ir( 9, (uint32_t)isr9 , 0x08, 0x8E);
    idt_install_ir(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_install_ir(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_install_ir(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_install_ir(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_install_ir(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_install_ir(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_install_ir(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_install_ir(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_install_ir(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_install_ir(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_install_ir(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_install_ir(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_install_ir(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_install_ir(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_install_ir(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_install_ir(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_install_ir(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_install_ir(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_install_ir(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_install_ir(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_install_ir(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_install_ir(31, (uint32_t)isr31, 0x08, 0x8E);

    idt_install_ir(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_install_ir(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_install_ir(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_install_ir(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_install_ir(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_install_ir(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_install_ir(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_install_ir(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_install_ir(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_install_ir(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_install_ir(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_install_ir(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_install_ir(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_install_ir(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_install_ir(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_install_ir(47, (uint32_t)irq15, 0x08, 0x8E);

    for (int i = 48; i < MAX_INTERRUPTS; i++)
    {
        idt_install_ir(i, (uint32_t)irq_fallback, 0x08, 0x8E);
    }

    idt_load((uint32_t)&__idtr);
}
