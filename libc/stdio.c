#include <stdio.h>
#include <syscall.h>

//&str, putcp, format, va);
static void stdio_format(void* out, int (*putc) (char, void*), char* str, va_list va);

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

void stdio_format(void* out, int (*putc) (char, void*), char* str, va_list va)
{
    while (*str)
    {
        if (*str != '%')
        {
            putc(*str, out);
        }
        else
        {
            str++;
            switch(*str)
            {
                case 's':
                {
                    const char* tmpStr = (const char*)va_arg(va);
                    while(*tmpStr)
                    {
                        putc(*tmpStr, out);
                    }
                }
                break;

                case 'c':
                {
                    char arg = va_arg(va);
                    putc(arg, out);
                }
                break;

                case 'i':
                case 'd':
                {
                    //MINI itoa (Maybe break it out?)
                    int arg = va_arg(va);
                    char buffer[10];
                    int digit = 0;

                    if(arg < 0)
                    {
                        putc('-', out);
                        arg *= -1;
                    }

                    if(arg == 0)
                    {
                        putc('0', out);
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
                            putc(buffer[i], out);
                        }
                    }
                }
                break;

                case 'x':
                case 'X':
                {
                    int arg = va_arg(va);
                    unsigned int nibble;

                    putc('0', out);
                    putc('x', out);
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
                        putc(nibble, out);
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

void printf(char* str, ...)
{
    va_list va;
    va_start(va, str);
    vprintf(str, va);
    va_end(va);
}

void vprintf(char* str, va_list va)
{
    stdio_format(stdout, fputc, str, va);
}

static int putcp(char c, void* p)
{
    *(*((char**)p))++ = c;
    return 0;
}

void sprintf(char* str, char* format, ...)
{
    va_list va;
    va_start(va, format);
    vsprintf(str, format, va);
    va_end(va);
}

void vsprintf(char* str, char* format, va_list va)
{
    stdio_format(&str, putcp, format, va);
    putcp('\0', &str);
}

int fputc (char character, void* file)
{
    return syscall_writedevice(*(int*)file, &character,1);
}

int fputs (const char* str, void* file )
{
    unsigned int size = 0;
    while(*str)
    {
        size += fputc((int)*str, file);
        str++;
    }
    return size;
}
