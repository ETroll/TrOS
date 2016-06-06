#include <tros/hal/serial.h>
#include <tros/hal/io.h>

#define PORT 0x3f8   /* COM1 */

static int serial_received();
static int is_transmit_empty();

void serial_init()
{
    pio_outb(0x00, PORT + 1);    // Disable all interrupts
    pio_outb(0x80, PORT + 3);    // Enable DLAB (set baud rate divisor)
    pio_outb(0x03, PORT + 0);    // Set divisor to 3 (lo byte) 38400 baud
    pio_outb(0x00, PORT + 1);    //                  (hi byte)
    pio_outb(0x03, PORT + 3);    // 8 bits, no parity, one stop bit
    pio_outb(0xC7, PORT + 2);    // Enable FIFO, clear them, with 14-byte threshold
    pio_outb(0x0B, PORT + 4);    // IRQs enabled, RTS/DSR set
}

static int serial_received()
{
    return pio_inb(PORT + 5) & 1;
}

static int is_transmit_empty()
{
    return pio_inb(PORT + 5) & 0x20;
}

char serial_readch()
{
    while (serial_received() == 0);
    return pio_inb(PORT);
}

void serial_putch(char a)
{
    while (is_transmit_empty() == 0);
    pio_outb(a, PORT);
}

void serial_puts(const char* str)
{
    while (*str)
    {
        serial_putch(*str);
        str++;
    }
}
