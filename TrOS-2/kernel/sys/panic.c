#include <tros/TrOS.h>

void kernel_panic(const char* message, cpu_registers_t* regs)
{
    //TODO: Create a more visible panic message using VGA..
    printk("KERNEL PANIC!\n%s\n", message);
    printk("Registers:\n");
    printk("eax=%x ebx=%x\n", regs->eax, regs->ebx);
    printk("ecx=%x edx=%x\n", regs->ecx, regs->edx);
    printk("esp=%x ebp=%x\n", regs->esp, regs->ebp);
    printk("Error code: %x\n", regs->err_code);
    printk("EFLAGS: %x\n", regs->eflags);
    printk("User ESP: %x\n", regs->useresp);
    printk("eip=%x\n", regs->eip);
    
    __asm("hlt;");
}
