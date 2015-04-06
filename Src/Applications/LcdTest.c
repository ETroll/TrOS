#include <Tros/Syscalls.h>
#include <stdio.h>
#include <string.h>

typedef enum
{
	ILI_BLACK = 0x000,
	ILI_BLUE = 0x001F,
	ILI_RED = 0xF800,
	ILI_GREEN = 0x07E0,
	ILI_CYAN = 0x07FF,
	ILI_MAGNETA = 0xF81F,
	ILI_YELLOW = 0xFFE0,
	ILI_WHITE = 0xFFFF
} ili_color_t;

void lcd_main(void)
{
	device_t device = open("lcd0");
	if(device > 0)
	{
		fprintf(stdout, "Opened device %d\n", device);
		while(1);
	}
	else {
		fprintf(stdout, "Error opening device\n");
		exit(0);
	}
}
