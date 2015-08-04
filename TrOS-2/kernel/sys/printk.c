#include <tros/hal/VGA.h>

extern void (*__putch)(char c);
extern void (*__puts)(const char* str);

void printk(char* str, ...)
{
	register int* arguments = (int *)(&str);

	while (*str)
	{
		if (*str != '%')
		{
			__putch(*str);
		}
		else
		{
			str++;
			arguments++;

			switch(*str)
			{
				case 's':
				{
					const char* str = (const char*)*arguments;
					__puts(str);
				}
				break;

				case 'c':
				{
					char arg = *arguments;
					__putch(arg);
				}
				break;

				case 'i':
				case 'd':
				{
					//MINI itoa (Maybe break it out?)
					int arg = *arguments;
					char buffer[10];
					int digit = 0;

					if(arg < 0)
					{
						__putch('-');
						arg *= -1;
					}

					if(arg == 0)
					{
						__putch('0');
					}
					else
					{
						while (arg > 0 && digit <= 9)
						{
							buffer[digit++] =  0x30 + (arg % 10);
							arg = arg / 10;
						}
						for(int i = digit-1; i>=0; i--)
						{
							__putch(buffer[i]);
						}
					}
				}
				break;

				case 'x':
				case 'X':
				{
					int arg = *arguments;
					unsigned int nibble;

					__putch('0');
					__putch('x');
					for(int i = 28; i >= 0; i-=4)
					{
						nibble = (arg >> i) & 0xF;
						if(nibble > 9)
						{
							nibble += 0x37;
						}
						else
						{
							nibble += 0x30;
						}
						__putch(nibble);
					}
				}
				/* no break */
				default:
				break;
			}
		}
		str++;
	}
}
