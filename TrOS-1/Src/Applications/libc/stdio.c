#include <stdio.h>
#include <Tros/Syscalls.h>

int fputc (int character, file_t* file)
{
	return write(*file, &character,1);
}

int fputs (const char* str, file_t* file )
{
	unsigned int size = 0;
	while(*str)
	{
		size += fputc((int)*str, file);
		str++;
	}
	return size;
}

int fprintf(file_t* file, char* str, ...)
{
	register int* arguments = (int *)(&str);
	int written = 0;
	while (*str)
	{
		if (*str != '%')
		{
			fputc(*str, file);
		}
		else
		{
			str++;
			arguments++;

			switch(*str)
			{
				case 's':
				{
					const char* arg = (const char*)*arguments;
					fputs(arg, file);
				}
				break;
				case 'd':
				{
					//MINI itoa (Maybe break it out?)
					int arg = *arguments;
					char buffer[10];
					int digit = 0;

					if(arg < 0){
						fputc('-', file);
						arg *= -1;
					}

					if(arg == 0)
					{
						fputc('0', file);
					}
					else {
						while (arg > 0 && digit <= 9) {
							buffer[digit++] =  0x30 + (arg % 10);
							arg = arg / 10;
						}

						for(int i = digit-1; i>=0; i--)
						{
							fputc(buffer[i], file);
						}
					}
				}
				break;
				case 'x':
				case 'X':
				{
					int arg = *arguments;
					unsigned int nibble;

					fputc('0', file);
					fputc('x', file);
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
						fputc(nibble, file);
					}
				}
				/* no break */
				default:
				break;
			}
		}
		str++;
	}
	return written;
}
