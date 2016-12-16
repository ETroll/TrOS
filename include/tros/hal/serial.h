#ifndef INCLUDE_SERIAL_H
#define INCLUDE_SERIAL_H

void serial_init();
char serial_readch();
void serial_putch(char a);
void serial_puts(const char* str);

#endif
