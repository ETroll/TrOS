#include <stdio.h>
#include <syscall.h>

//very primitive atoi implementation, no real edge-cases accounted for
int atoi(const char* str)
{
    int res = 0;
    int counter = 0;
    int sign = 1;

    if(str[0] == '-')
    {
        sign = -1;
        counter = 1;
    }

    while(str[counter] != '\0')
    {
        res = res*10 + (str[counter] - 0x30); //0x30 -- Ascii '0'
        counter++;
    }

    return sign*res;
}

void printf(char* str, ...)
{
    register int* arguments = (int *)(&str);

    while (*str)
    {
        if (*str != '%')
        {
            fputc(*str, stdout);
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
                    fputs(str, stdout);
                }
                break;

                case 'c':
                {
                    char arg = *arguments;
                    fputc(arg, stdout);
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
                        fputc('-', stdout);
                        arg *= -1;
                    }

                    if(arg == 0)
                    {
                        fputc('0', stdout);
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
                            fputc(buffer[i], stdout);
                        }
                    }
                }
                break;

                case 'x':
                case 'X':
                {
                    int arg = *arguments;
                    unsigned int nibble;

                    fputc('0', stdout);
                    fputc('x', stdout);
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
                        fputc(nibble, stdout);
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

int fputc (int character, file_t* file)
{
    return syscall_writedevice(*file, &character,1);
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
