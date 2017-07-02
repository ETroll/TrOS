#include <tros/hal/pic.h>
#include <tros/hal/io.h>

#define PIC1_CREG 0x20
#define PIC2_CREG 0xA0
#define PIC1_DREG 0x21
#define PIC2_DREG 0xA1

void pic_initialize()
{

    pio_outb(PIC1_CREG, 0x11);
    pio_outb(PIC2_CREG, 0x11);

    //Mapping IRQ0 -> IRQ32 und so weiter
    pio_outb(PIC1_DREG, 0x20);
    pio_outb(PIC2_DREG, 0x28);

    //Connect the PICs
    pio_outb(PIC1_DREG, 0x04);
    pio_outb(PIC2_DREG, 0x02);

    //Set X86 mode
    pio_outb(PIC1_DREG, 0x01);
    pio_outb(PIC2_DREG, 0x01);

    pio_outb(PIC1_DREG, 0x00);
    pio_outb(PIC2_DREG, 0x00);
}

void pic_eoi(unsigned int irq)
{
    if (irq >= 40)
    {
        pio_outb(PIC2_CREG, 0x20);
    }
    pio_outb(PIC1_CREG, 0x20);
}
