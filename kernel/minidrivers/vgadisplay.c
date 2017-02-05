#include <tros/driver.h>
#include <tros/hal/io.h>
#include <tros/tros.h>

#define VGA_CURSOR_IREG 0x3D4
#define VGA_CURSOR_DREG 0x3D5
#define VGA_ROWS 25
#define VGA_COLS 80
#define VGA_MEM_SIZE VGA_COLS + (VGA_ROWS*VGA_COLS)

#define IOCTL_VGA_COLOR         1
#define IOCTL_VGA_CURSER_POS    2
#define IOCTL_VGA_TOGGLE_CURSOR 3
#define IOCTL_VGA_CLEAR_MEM     4

static unsigned char _vga_isopen = 0;
static unsigned char _vga_xpos = 0;
static unsigned char _vga_ypos = 0;
static unsigned char _vga_show_cursor = 0;

static unsigned char _vga_current_color = 0;
static unsigned short* _vga_mem = (unsigned short*)0xB8000;

int vga_drv_read(int* buffer, unsigned int count);
int vga_drv_write(int* buffer, unsigned int count);
int vga_drv_ioctl(unsigned int num, unsigned int param);
int vga_drv_open();
void vga_drv_close();

//Internal functions
void vga_drv_update_cursor();

static driver_generic_t _vga_driver =
{
    .read = vga_drv_read,
    .write = vga_drv_write,
    .ioctl = vga_drv_ioctl,
    .open = vga_drv_open,
    .close = vga_drv_close
};

int vga_driver_initialize()
{
    device_driver_t drv = {
        .name = "vga",
        .type = DRV_GENERIC,
        .driver = &_vga_driver
    };

    _vga_isopen = 0;
    _vga_show_cursor = 1;
    _vga_current_color = 0x0F;

    printk("** Installing 80x25 VGA driver - ");
	return driver_register(&drv);
}

int vga_drv_read(int* buffer, unsigned int count)
{
    int bytes_read = 0;
    unsigned int memloc = _vga_xpos + (_vga_ypos * VGA_COLS);

    for(; bytes_read<count; bytes_read++)
    {
        if(memloc < VGA_MEM_SIZE)
        {
            buffer[bytes_read] = (int)_vga_mem[memloc];
            memloc++;
        }
        else { break; }
    }
    return bytes_read;
}

int vga_drv_write(int* buffer, unsigned int count)
{
    int bytes_written = 0;
    for(; bytes_written<count; bytes_written++)
    {
        int data = buffer[bytes_written];
        int vgadata = (data & 0xFFFF);
        unsigned char x = (unsigned char)((data & 0xFF000000) >> 24);
        unsigned char y = (unsigned char)((data & 0x00FF0000) >> 16);

        if(x > VGA_COLS || y > VGA_ROWS)
        {
            printk("VGA ERROR: X %d, Y %d, vgadata %x, data %x\n", x, y, vgadata, data);
        }

        if(x < VGA_COLS && y < VGA_ROWS)
        {
            unsigned int memloc = x + (y * VGA_COLS);
            _vga_mem[memloc] = (short)vgadata;
        }
    }
    return bytes_written;
}

int vga_drv_ioctl(unsigned int ioctl_num, unsigned int param)
{
    switch (ioctl_num)
    {
        case IOCTL_VGA_COLOR:
            _vga_current_color = (unsigned char)param;
        break;
        case IOCTL_VGA_TOGGLE_CURSOR:
        {
            if(param > 0)
            {
                _vga_show_cursor = 1;
                vga_drv_update_cursor();
            }
            else
            {
                _vga_show_cursor = 0;
                //Just moving the cursor out of screen
                pio_outb(0x0F, VGA_CURSOR_IREG);
                pio_outb(0xFF, VGA_CURSOR_DREG);

                pio_outb(0x0E, VGA_CURSOR_IREG);
                pio_outb(0xFF, VGA_CURSOR_DREG);
            }
        } break;
        case IOCTL_VGA_CURSER_POS:
        {
            unsigned char x = (unsigned char)((param & 0xFF00) >> 8);
            unsigned char y = (unsigned char)(param & 0xFF);

            if(x < VGA_COLS && y < VGA_ROWS)
            {
                _vga_xpos = x;
                _vga_ypos = y;
            }
        } break;
        case IOCTL_VGA_CLEAR_MEM:
        {
            for (int i=0; i<(VGA_COLS*VGA_ROWS); i++)
            {
                _vga_mem[i] =  (_vga_current_color << 8) | 0x0020;
            }
            _vga_xpos = 0;
            _vga_ypos = 0;
        } break;
    }
    return TROS_DRIVER_OK;
}

int vga_drv_open()
{
    if(!_vga_isopen)
    {
        return TROS_DRIVER_OK;
    }
    else
    {
        return TROS_DRIVER_WASOPEN;
    }
}

void vga_drv_close()
{
    _vga_isopen = 0;
}

void vga_drv_update_cursor()
{
    if(_vga_show_cursor)
    {
        unsigned int mempos = (VGA_COLS * _vga_ypos) + _vga_xpos;
        unsigned char mempos_low = mempos & 0xFF;
        unsigned char mempos_high = (mempos >> 8) & 0xFF;

        pio_outb(0x0F, VGA_CURSOR_IREG);
        pio_outb(mempos_low, VGA_CURSOR_DREG);

        pio_outb(0x0E, VGA_CURSOR_IREG);
        pio_outb(mempos_high, VGA_CURSOR_DREG);
    }
}
