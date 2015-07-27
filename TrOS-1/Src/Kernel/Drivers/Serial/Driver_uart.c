#include <Tros/Drivers.h>
#include <Tros/Mmio.h>
#include <Tros/Gpio.h>
#include <Tros/IRQ.h>
#include <Tros/Kernel.h>
#include <Tros/Arch/Raspberry.h>
#include <Tros/Utils.h>

int uart_read (char* buffer, unsigned int count);
int uart_write (const char* buffer, unsigned int count);
int uart_writechar (char c);
int uart_peek (void);
int uart_open (void);
int uart_close (void);
int uart_irq_handler(irq_t irq);

int uart_is_opened;
struct ringbuffer _uart_data;

struct serial_driver __uartdriver = {
	.read = uart_read,
	.write = uart_write,
	.writechar = uart_writechar,
	.peek = uart_peek,
	.open = uart_open,
	.close = uart_close
};

int uart_init_driver()
{
	uart_is_opened = 0;

	return drivers_register_serial(&__uartdriver, "uart0");
}

int uart_peek (void)
{
	return rb_len(&_uart_data);
}

int uart_read (char *buffer, unsigned int count)
{
	//TODO: Add kernel functionality for READ_WAIT
	unsigned int read = 0;
	while(read < count)
	{
		if(rb_len(&_uart_data) > 0)
		{
			rb_pop(&_uart_data, &buffer[read++]);
		}
	}
	return read;
}

int uart_write (const char *buffer, unsigned int count)
{
	unsigned int written = 0;
	while(written < count)
	{
		written += uart_writechar(buffer[written]);
	}
	return written;
}

int uart_writechar (char c)
{
	while(1)
	{
		unsigned int reg = mmio_read(UART0_FLAG_REG);
		if((reg & 0x20) == UART_READY)
		{
			break;
		}
	}
	mmio_write(UART0_IO_REG, (unsigned int)c);
	return 1;
}

int uart_open ()
{
	if(!uart_is_opened)
	{
		mmio_write(UART0_CTRL_REG, 0x00000000);

		gpio_enable_pin(14, GPIO_ALT0);
		gpio_enable_pin(15, GPIO_ALT0);

		mmio_write(GPIO_PUD, 0x00000000);
		for(int i=0; i<150; i++) dummy(i);

		mmio_write(GPIO_PUD_CLK0, (1<<14) | (1<<15));
		for(int i=0; i<150; i++) dummy(i);

		mmio_write(GPIO_PUD_CLK0, 0x00000000);
		mmio_write(UART0_INTR_ICR, 0x7FF);
		mmio_write(UART0_INT_BAUD, 1);
		mmio_write(UART0_FRAC_BAUD, 40);

		//mmio_write(UART0_LINE_CTRL_REG,0x70);
		//mmio_write(UART0_CTRL_REG,0x301);

		mmio_write(UART0_LINE_CTRL_REG, (1 << 4) | (1 << 5) | (1 << 6));
		mmio_write(UART0_INTR_MSC, (1 << 4) | (1 << 6)) ;	//Just on RX and RX timeout
		//mmio_write(UART0_INTR_MSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
		//	                            							 (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
		mmio_write(UART0_CTRL_REG, (1 << 0) | (1 << 8) | (1 << 9));

		irq_handler_free(IRQ_UART);
		irq_handler_register(IRQ_UART, uart_irq_handler);

		rb_init(&_uart_data);

		uart_is_opened = 1;
	}
	return 1;
}

int uart_irq_handler(irq_t irq)
{
	if(uart_is_opened)
	{
		unsigned int data;
		unsigned int intr_status = mmio_read(UART0_INTR_MIS);
		unsigned int num_bytes = 0;
		if ((intr_status & (1 << 4)) || (intr_status & (1 << 6)) )
		{
			do
			{
		         data = mmio_read(UART0_IO_REG);
		         rb_push(&_uart_data, (char)(data & 0xFF));
		         num_bytes++;
			}
			while ((mmio_read(UART0_FLAG_REG) & (1<<4) ) == 0);
		}

		mmio_write(UART0_INTR_ICR, intr_status);
		return 1;
	}
	else
	{
		return 0;
	}
}

int uart_close ()
{
	//clean up UART use
	irq_handler_free(IRQ_UART);
	uart_is_opened = 0;
	return 1;
}
