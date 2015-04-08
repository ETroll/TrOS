#include <Tros/Mmio.h>
#include <Tros/Arch/Raspberry.h>
#include <Tros/Gpio.h>

void gpio_enable_pin(unsigned int pin, gpio_direction_t dir)
{
	unsigned int func_offset = pin / 10;

	unsigned int func_reg = GPIO_FUNCSEL_BASE + (func_offset*4);
	unsigned int pin_offset = (pin % 10) * 3;

	unsigned int data = mmio_read(func_reg);
	data &= ~(0x07 << pin_offset);
	data |= (dir << pin_offset);

	mmio_write(func_reg, data);
}

void gpio_set_pin(unsigned int pin)
{
	unsigned int offset = pin / 32;

	if(offset == 0)
	{
		mmio_write(GPIO_SET_0, (1 << pin));
	}
	else
	{
		mmio_write(GPIO_SET_0, (1 << (pin-32)));
	}
}

void gpio_clear_pin(unsigned int pin)
{
	unsigned int offset = pin / 32;

	if(offset == 0)
	{
		mmio_write(GPIO_SET_0, (0 << pin));
	}
	else
	{
		mmio_write(GPIO_SET_0, (0 << (pin-32)));
	}
}

