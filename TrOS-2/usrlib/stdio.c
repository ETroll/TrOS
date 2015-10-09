#include <stdio.h>

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
