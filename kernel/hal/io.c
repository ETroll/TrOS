#include <tros/hal/io.h>


unsigned char pio_inb(unsigned short port)
{
    unsigned char ret;
    __asm("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void pio_outb(unsigned short port, unsigned char value)
{
    __asm("outb %0, %1" : : "a"(value), "Nd"(port));
}

unsigned short pio_inw(unsigned short port)
{
    unsigned short ret;
    __asm("inw %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

void pio_outw(unsigned short port, unsigned short value)
{
    __asm("outw %0, %1" : : "a"(value), "d"(port));
}
