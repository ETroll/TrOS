#include <tros/tros.h>
#define FLAGSET(var,pos) ((var >> pos) & 0x01)

void kernel_panic(const char* message, cpu_registers_t* regs)
{
    //TODO: Create a more visible panic message using VGA..
    printk("KERNEL PANIC! -- %s\n", message);

    if(regs != 0)
    {
        printk("Registers:\n");
        printk("EAX=%x EBX=%x ECX=%x EDX=%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
        printk("ESP=%x EBP=%x EIP=%x\n", regs->esp, regs->ebp, regs->eip);
        printk("Error code: %x\n", regs->err_code);
        printk("User ESP:   %x\n", regs->useresp);
        printk("#-EFLAGS----%x---------------------------------#\n",  regs->eflags);
        printk("|CF|PF|AF|ZF|SF|TF|IF|DF|OF|IOPL|NT|RF|VM|AC|VIF|VIP|ID|\n");
        printk("| %i| %i| %i| %i| %i| %i| %i| %i| %i|   %i| %i| %i| %i| %i|  %i|  %i| %i|\n",
            FLAGSET(regs->eflags, 0),
            FLAGSET(regs->eflags, 1),
            FLAGSET(regs->eflags, 4),
            FLAGSET(regs->eflags, 6),
            FLAGSET(regs->eflags, 7),
            FLAGSET(regs->eflags, 8),
            FLAGSET(regs->eflags, 9),
            FLAGSET(regs->eflags, 10),
            FLAGSET(regs->eflags, 11),
            (regs->eflags >> 12) & 0x11,
            FLAGSET(regs->eflags, 14),
            FLAGSET(regs->eflags, 16),
            FLAGSET(regs->eflags, 17),
            FLAGSET(regs->eflags, 18),
            FLAGSET(regs->eflags, 19),
            FLAGSET(regs->eflags, 20),
            FLAGSET(regs->eflags, 21)
        );
        printk("#------------------------------------------------------#\n");
    }

    // int base_pointer = 0;
    // __asm("mov %%ebp, %0;" : "=a"(base_pointer));
    //
    // printk("Stacktrace: \n");
    // for(int i = 0; i<3; i++) {
    //     void* ebp = (void*) base_pointer;
    //     void* caller = (void*) (base_pointer+4);
    //
    //     printk("EBP: %x - M: %x\n", base_pointer, caller);
    //
    //     // base_pointer =
    // }

    __asm("cli;");
    __asm("hlt;");
}
