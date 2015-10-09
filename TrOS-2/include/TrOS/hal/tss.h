// tss.h
// TSS x86


#ifndef INCLUDE_TROS_TSS_H
#define INCLUDE_TROS_TSS_H

typedef struct  {
    unsigned int prevTss;
    unsigned int esp0;
    unsigned int ss0;
    unsigned int esp1;
    unsigned int ss1;
    unsigned int esp2;
    unsigned int ss2;
    unsigned int cr3;
    unsigned int eip;
    unsigned int eflags;
    unsigned int eax;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
    unsigned int es;
    unsigned int cs;
    unsigned int ss;
    unsigned int ds;
    unsigned int fs;
    unsigned int gs;
    unsigned int ldt;
    unsigned short trap;
    unsigned short iomap;
} __attribute__((packed)) tss_entry_t;


//extern void tss_flush(unsigned short sel);
extern void tss_flush();
extern void tss_flush_old();
void tss_set_stack(unsigned short kernelSS, unsigned short kernelESP);
void tss_install(unsigned int sel, unsigned short kernelSS, unsigned short kernelESP);

#endif
