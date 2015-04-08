#ifndef _GPIO_H
#define _GPIO_H

typedef enum
{
	GPIO_IN = 0x00,
	GPIO_OUT = 0x01,
	GPIO_ALT0 = 0x04,
	GPIO_ALT1 = 0x05,
	GPIO_ALT2 = 0x06,
	GPIO_ALT3 = 0x07,
	GPIO_ALT4 = 0x03,
	GPIO_ALT5 = 0x02
} gpio_direction_t;

void gpio_enable_pin(unsigned int pin, gpio_direction_t dir);
void gpio_set_pin(unsigned int pin);
void gpio_clear_pin(unsigned int pin);

#endif
