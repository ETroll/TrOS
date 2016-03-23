#include <tros/process.h>
#include <tros/kheap.h>
#include <tros/tros.h>

static process_t* current_process;
static process_t main_process;
static process_t other_process;

static void otherMain() {
    while(1) {
        printk("Hello multitasking world!\n"); // Not implemented here...
        process_preempt();
    }
}

void process_preempt()
{
    //BOCHS_DEBUG;
    process_t *last = current_process;
    current_process = current_process->next;
    printk("SWCH TO EIP: %x CR3: %x ESP: %x EFLAG: %x\n",
        current_process->regs.eip,
        current_process->regs.cr3,
        current_process->regs.esp,
        current_process->regs.eflags);
    process_switch(&last->regs, &current_process->regs);
}

void process_create(process_t *task, void (*main)(), unsigned int flags, unsigned int* pagedir)
{
    printk("Creating process - Main: %x Flags: %x CR3: %x\n", main, flags, pagedir);
    task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.eflags = flags;
    task->regs.eip = (unsigned int)main;
    task->regs.cr3 = (unsigned int)pagedir;
    task->regs.esp = (unsigned int)kmalloc(0x1000) + 0xFFC; // Not implemented here
    task->next = 0;
}

void process_init() {
    // Get EFLAGS and CR3
    __asm("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(main_process.regs.cr3)::"%eax");
    __asm("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(main_process.regs.eflags)::"%eax");

    process_create(&other_process, &otherMain, main_process.regs.eflags, (uint32_t*)main_process.regs.cr3);
    main_process.next = &other_process;
    other_process.next = &main_process;

    current_process = &main_process;
}
