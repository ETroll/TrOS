// cpu.h
// X86 CPU methods

#ifndef INCLUDE_TROS_CPU_H
#define INCLUDE_TROS_CPU_H

typedef struct
{
    char* name;
    int model;
    int family;
    int type;
    int brand;
} cpuinfo_t;


void cpu_initialize();
cpuinfo_t cpu_get_info();

#endif
