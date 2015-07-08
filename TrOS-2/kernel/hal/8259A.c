#include <TrOS/hal/8259A.h>
#include <TrOS/hal/io.h>

void init_8259A()
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

void eoi_8259A()
{
    outb(0x20, PIC1_CREG);
}
