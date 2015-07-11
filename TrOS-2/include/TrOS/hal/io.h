// io.h
// Basic PMIO and MMIO methods

//TODO:
//  Move out from HAL and wrap port io and mm io for the underlying arch

#ifndef INCLUDE_TROS_IO_H
#define INCLUDE_TROS_IO_H

unsigned char inb(unsigned short int port);
void outb(unsigned char value, unsigned short int port);

#endif
