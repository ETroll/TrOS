#include <tros/hal/serial.h>
#include <tros/hal/io.h>

#define PORT 0x3f8   /* COM1 */

static int serial_received();
static int is_transmit_empty();

void serial_init()
{
    pio_outb(PORT + 1, 0x00);    // Disable all interrupts
    pio_outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    pio_outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    pio_outb(PORT + 1, 0x00);    //                  (hi byte)
    pio_outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    pio_outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    pio_outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
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
    pio_outb(PORT, a);
}

void serial_puts(const char* str)
{
    while (*str)
    {
        serial_putch(*str);
        str++;
    }
}
