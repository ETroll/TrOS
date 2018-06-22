// io.h
// Basic PMIO and MMIO methods

//TODO:
//  Move out from HAL and wrap port io and mm io for the underlying arch

#ifndef INCLUDE_TROS_IO_H
#define INCLUDE_TROS_IO_H

unsigned char pio_inb(unsigned short port);
unsigned short pio_inw(unsigned short port);
unsigned int pio_indw(unsigned short port);

void pio_outb(unsigned short port, unsigned char value);
void pio_outw(unsigned short port, unsigned short value);
void pio_outdw(unsigned short port, unsigned int value);



#endif
