// Kernel.c
// TrOS kernel - Main entry

#include <VGA.h>

extern void kernel_main(void);

void kernel_early()
{
    // asm(".intel_syntax noprefix");
    // asm("cli");
    // asm("mov ax, 0x10");
    // asm("mov ds, ax");
    // asm("mov es, ax");
    // asm("mov fs, ax");
    // asm("mov gs, ax");
    // asm("mov ss, ax");
    // asm("mov esp, 0x90000");
    // asm("mov ebp, esp");
    // asm("push ebp");
}

int main()
{
    kernel_main();
    kernel_early();
    return 1;
}
