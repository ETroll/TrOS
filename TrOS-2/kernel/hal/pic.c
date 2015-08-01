#include <tros/hal/pic.h>
#include <tros/hal/io.h>

#define PIC1_CREG 0x20
#define PIC2_CREG 0xA0
#define PIC1_DREG 0x21
#define PIC2_DREG 0xA1

void pic_initialize()
{

    pio_outb(0x11, PIC1_CREG);
    pio_outb(0x11, PIC2_CREG);

    //Mapping IRQ0 -> IRQ32 und so weiter
    pio_outb(0x20, PIC1_DREG);
    pio_outb(0x28, PIC2_DREG);

    //Connect the PICs
    pio_outb(0x04, PIC1_DREG);
    pio_outb(0x02, PIC2_DREG);

    //Set X86 mode
    pio_outb(0x01, PIC1_DREG);
    pio_outb(0x01, PIC2_DREG);

    pio_outb(0x00, PIC1_DREG);
    pio_outb(0x00, PIC2_DREG);
}

void pic_eoi(unsigned int irq)
{
    if (irq >= 40)
    {
        pio_outb(0x20, PIC2_CREG);
    }
    pio_outb(0x20, PIC1_CREG);
}
