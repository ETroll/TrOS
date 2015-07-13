#include <TrOS/hal/cpu.h>

#define cpuid(in, a, b, c, d) __asm__("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));

void cpu_initialize()
{
    //Set up stuff like SMP?
    __asm("nop;");
}

cpuinfo_t cpu_get_info()
{
    __asm("nop;");
    cpuinfo_t info;
    return info;
}
