// io.h
// Basic PMIO and MMIO methods

#ifndef INCLUDE_TROS_IO_H
#define INCLUDE_TROS_IO_H

unsigned char inb(unsigned short int port);
void outb(unsigned char value, unsigned short int port);

#endif
