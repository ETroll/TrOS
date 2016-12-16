#include <tros/hal/io.h>


unsigned char pio_inb(unsigned short int port)
{
    unsigned char ret;
    __asm("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void pio_outb(unsigned char value, unsigned short int port)
{
    __asm("outb %0, %1" : : "a"(value), "Nd"(port));
}
unsigned char mmio_inb(unsigned short int port)
{
    //TODO
    return 0;
}
void mmio_outb(unsigned char value, unsigned short int port)
{
    //TODO
}
