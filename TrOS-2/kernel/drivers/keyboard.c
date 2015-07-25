#include <tros/driver.h>
#include <tros/hal/io.h>
#include <tros/irq.h>
#include <tros/tros.h>
#include <string.h>

#define KEY_DEVICE  0x60
#define KEY_PENDING 0x64

int kbd_read(char* buffer, unsigned int count);
int kbd_ioctl(unsigned int num, unsigned long param);
int kbd_open();
int kbd_close();
void kbd_irq_handler(cpu_registers_t* regs);

static driver_hid_t __kbdriver = {
    .read = kbd_read,
    .ioctl = kbd_ioctl
};

int kbd_driver_initialize()
{
    device_driver_t drv = {
        .name = "kbd",
        .type = DRV_HID,
        .driver = &__kbdriver
    };

    printk("Initalizing generic keyboard driver\n");
    irq_register_handler(33, &kbd_irq_handler);
	return driver_register(&drv);
}

void kbd_driver_remove()
{
    
}

int kbd_read(char* buffer, unsigned int count)
{
    return 0;
}

int kbd_ioctl(unsigned int num, unsigned long param)
{
    return 0;
}

void kbd_irq_handler(cpu_registers_t* regs)
{
    while(inb(KEY_PENDING) & 2);
    unsigned char scancode = inb(KEY_DEVICE);

    printk("KBD Scancode: %x\n", scancode);

    irq_eoi(1);
}
