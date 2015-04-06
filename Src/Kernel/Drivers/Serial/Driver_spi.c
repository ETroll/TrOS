#include <Tros/Drivers.h>
#include <Tros/Mmio.h>
#include <Tros/Gpio.h>
#include <Tros/Utils.h>
#include <Tros/Arch/Raspberry.h>

int spi_write (const char *buffer, unsigned int count);
int spi_writechar (char c);
int spi_open (void);
int spi_close (void);

int spi_is_open;

struct serial_driver __spidriver = {
	.read = 0,
	.write = spi_write,
	.writechar = spi_writechar,
	.open = spi_open,
	.close = spi_close
};

int spi_init_driver()
{
	spi_is_open = 0;

	return drivers_register_serial(&__spidriver, "spi0");
}

int spi_write (const char *buffer, unsigned int count)
{
	unsigned int written = 0;
	while(written < count)
	{
		written += spi_writechar(buffer[written]);
	}
	return written;
}

int spi_writechar (char c)
{
	mmio_write(SPI0_CTRL_STATUS,0x000000B0); //TA=1 cs asserted
	while(TRUE)
	{
		if(mmio_read(SPI0_CTRL_STATUS) & (1<<18)) break; //TXD
	}
	mmio_write(SPI0_FIFO, c & 0xFF);
	while(TRUE)
	{
		if(mmio_read(SPI0_CTRL_STATUS) & (1<<16)) break;
	}
	//while(1) if(GET32(AUX_SPI0_CS)&(1<<17)) break; //should I wait for this?
	mmio_write(SPI0_CTRL_STATUS, 0x00000000); //cs0 comes back up
	return 1;
}

int spi_open ()
{
	if(!spi_is_open)
	{

		unsigned int aux_reg = mmio_read(AUX_ENABLES);
		aux_reg |= 2; //enable spi0
		mmio_write(AUX_ENABLES,aux_reg);
		gpio_enable_pin(9, GPIO_ALT0);
		gpio_enable_pin(8, GPIO_ALT0);
		gpio_enable_pin(7, GPIO_ALT0);

		gpio_enable_pin(10, GPIO_ALT0);
		gpio_enable_pin(11, GPIO_ALT0);

		gpio_enable_pin(25, GPIO_OUT);

		mmio_write(SPI0_CTRL_STATUS, 0x0000030);
		mmio_write(SPI0_CLK_DIVIDER, 32);

		spi_is_open = 1;
	}
	return 1;
}

int spi_close ()
{
	spi_is_open = 0;
	return 1;
}


