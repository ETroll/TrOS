#include <tros/driver.h>
#include <tros/hal/io.h>
#include <tros/tros.h>

#define VGA_CURSOR_IREG 0x3D4
#define VGA_CURSOR_DREG 0x3D5
#define VGA_ROWS 25
#define VGA_COLS 80
#define VGA_MEM_SIZE (VGA_COLS*2) + (VGA_ROWS*VGA_COLS*2)

#define IOCTL_VGA_COLOR         1
#define IOCTL_VGA_SCROLL_UP     2
#define IOCTL_VGA_SCROLL_DOWN   3
#define IOCTL_VGA_TOGGLE_CURSOR 4
#define IOCTL_VGA_CLEAR_MEM     5
#define IOCTL_VGA_SHOULD_SCROLL 6

static unsigned char __vga_isopen = 0;
static unsigned char __vga_xpos = 0;
static unsigned char __vga_ypos = 0;
static unsigned char __vga_should_autoscroll = 0;
static unsigned char __vga_show_cursor = 0;

static unsigned char __vga_current_color = 0;
static unsigned char* __vga_mem = (unsigned char*)0xB8000;

int vga_drv_read(char* buffer, unsigned int count);
int vga_drv_write(char* buffer, unsigned int count);
int vga_drv_ioctl(unsigned int num, unsigned int param);
int vga_drv_open();
int vga_drv_seek(unsigned int pos);
void vga_drv_close();

//Internal functions
void vga_drv_update_cursor();
void vga_drv_scrollup(unsigned int amount);
void vga_drv_scrolldown(unsigned int amount);

static driver_char_t __vga_driver =
{
    .read = vga_drv_read,
    .write = vga_drv_write,
    .seek = vga_drv_seek,
    .ioctl = vga_drv_ioctl,
    .open = vga_drv_open,
    .close = vga_drv_close
};

int vga_driver_initialize()
{
    device_driver_t drv = {
        .name = "vga",
        .type = DRV_CHAR,
        .driver = &__vga_driver
    };

    __vga_isopen = 0;
    __vga_show_cursor = 1;
    //Black backround, White text
    __vga_current_color = 0x0F;

    printk("** Initalizing 80x25 VGA driver - ");
	return driver_register(&drv);
}


int vga_drv_read(char* buffer, unsigned int count)
{
    //NOTE: Something "Special" about this read is that it does not move
    //      any pointers at all like a normal read would
    int bytes_read = 0;
    unsigned int memloc = (__vga_xpos * 2)+(__vga_ypos * VGA_COLS *2);

    for(; bytes_read<count; bytes_read++)
    {
        if(memloc < VGA_MEM_SIZE)
        {
            buffer[bytes_read] = __vga_mem[memloc];
            memloc+=2;
        }
        else
        {
            break;
        }
    }
    return bytes_read;
}

int vga_drv_write(char* buffer, unsigned int count)
{
    int bytes_written = 0;
    for(; bytes_written<count; bytes_written++)
    {
        unsigned char data = buffer[bytes_written];

        if (data == '\n'|| data == '\r' || __vga_xpos > (VGA_COLS-1))
        {
            __vga_ypos++;
            __vga_xpos = 0;

            if(__vga_should_autoscroll && __vga_ypos >= VGA_ROWS)
            {
                vga_drv_scrollup(1);
                __vga_ypos = (VGA_ROWS-1);
            }
        }
        else
        {
            unsigned int memloc = (__vga_xpos * 2)+(__vga_ypos * VGA_COLS *2);

            __vga_mem[memloc] = data;
            __vga_mem[memloc+1] = __vga_current_color;
            __vga_xpos++;
        }
        vga_drv_update_cursor();
    }

    return bytes_written;
}

int vga_drv_ioctl(unsigned int ioctl_num, unsigned int param)
{
    switch (ioctl_num)
    {
        case IOCTL_VGA_COLOR:
            __vga_current_color = (unsigned char)param;
        break;
        case IOCTL_VGA_SCROLL_UP:
            vga_drv_scrollup(param);
        break;
        case IOCTL_VGA_SCROLL_DOWN:
            vga_drv_scrolldown(param);
        break;
        case IOCTL_VGA_TOGGLE_CURSOR:
        {
            if(param > 0)
            {
                __vga_show_cursor = 1;
                vga_drv_update_cursor();
            }
            else
            {
                __vga_show_cursor = 0;
                //Just moving the cursor out of screen
                pio_outb(0x0F, VGA_CURSOR_IREG);
                pio_outb(0xFF, VGA_CURSOR_DREG);

                pio_outb(0x0E, VGA_CURSOR_IREG);
                pio_outb(0xFF, VGA_CURSOR_DREG);
            }
        } break;
        case IOCTL_VGA_CLEAR_MEM:
        {
            for (int i=0; i<((VGA_COLS*2)*VGA_ROWS); i+=2)
            {
                __vga_mem[i] = ' ';
                __vga_mem[i+1] = __vga_current_color;
            }
            __vga_xpos = 0;
            __vga_ypos = 0;
        } break;
        case IOCTL_VGA_SHOULD_SCROLL:
        {
            if(param > 0)
            {
                __vga_should_autoscroll = 1;
            }
            else
            {
                __vga_should_autoscroll = 0;
            }
        } break;
    }
    return TROS_DRIVER_OK;
}

int vga_drv_open()
{
    if(!__vga_isopen)
    {
        return TROS_DRIVER_OK;
    }
    else
    {
        return TROS_DRIVER_WASOPEN;
    }
}

int vga_drv_seek(unsigned int pos)
{
    if(pos < (VGA_COLS * VGA_ROWS))
    {
        __vga_ypos = pos / VGA_COLS;
        __vga_xpos = pos % VGA_COLS;

        vga_drv_update_cursor();
        return TROS_DRIVER_OK;
    }
    else
    {
        return TROS_DRIVER_ERROR;
    }
}

void vga_drv_close()
{
    __vga_isopen = 0;
}

void vga_drv_update_cursor()
{
    if(__vga_show_cursor)
    {
        unsigned int mempos = (VGA_COLS * __vga_ypos) + __vga_xpos;
        unsigned char mempos_low = mempos & 0xFF;
        unsigned char mempos_high = (mempos >> 8) & 0xFF;

        pio_outb(0x0F, VGA_CURSOR_IREG);
        pio_outb(mempos_low, VGA_CURSOR_DREG);

        pio_outb(0x0E, VGA_CURSOR_IREG);
        pio_outb(mempos_high, VGA_CURSOR_DREG);
    }
}

void vga_drv_scrollup(unsigned int amount)
{
    for (int i = 0; i < (VGA_ROWS-amount)*(VGA_COLS*2); i+=2)
    {
        __vga_mem[i] = __vga_mem[i+(VGA_COLS*2*amount)];
        __vga_mem[i+1] = __vga_mem[i+(VGA_COLS*2*amount)+1];
    }

    for (int i = (VGA_ROWS-amount)*(VGA_COLS*2);
        i < (VGA_COLS*2)*VGA_ROWS;
        i+=2)
    {
        __vga_mem[i] = ' ';
        __vga_mem[i+1] = __vga_current_color;
    }

}

void vga_drv_scrolldown(unsigned int amount)
{
    for (int i = (VGA_ROWS*VGA_COLS*2);
         i >= amount*(VGA_COLS*2); i-=2)
    {
        __vga_mem[i] = __vga_mem[i-(VGA_COLS*2*amount)];
        __vga_mem[i+1] = __vga_mem[i-(VGA_COLS*2*amount)+1];
    }

    for (int i = 0;
        i < amount*(VGA_COLS*2);
        i+=2)
    {
        __vga_mem[i] = ' ';
        __vga_mem[i+1] = __vga_current_color;
    }
}
