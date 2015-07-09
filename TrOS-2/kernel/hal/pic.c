#include <TrOS/hal/pic.h>
#include <TrOS/hal/io.h>

#define PIC1_CREG 0x20
#define PIC2_CREG 0xA0
#define PIC1_DREG 0x21
#define PIC2_DREG 0xA1

void pic_initialize()
{
    outb(0x11, PIC1_CREG);
    outb(0x11, PIC2_CREG);

    outb(0x20, PIC1_DREG);
    outb(0x28, PIC2_DREG);

    outb(0x04, PIC1_DREG);
    outb(0x02, PIC2_DREG);

    outb(0x01, PIC1_DREG);
    outb(0x01, PIC2_DREG);

    outb(0x00, PIC1_DREG);
    outb(0x00, PIC2_DREG);
}

void pic_eoi()
{
    outb(0x20, PIC1_CREG);
}
