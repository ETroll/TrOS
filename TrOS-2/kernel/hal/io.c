#include <TrOS/hal/io.h>


unsigned char inb(unsigned short int port)
{
    unsigned char ret;
    __asm("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(unsigned char value, unsigned short int port)
{
    __asm("outb %0, %1" : : "a"(value), "Nd"(port));
}
