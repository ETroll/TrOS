#include <tros/driver.h>
#include <tros/hal/io.h>
#include <tros/tros.h>

#define TEXTMODE_CURSOR_IREG 0x3D4
#define TEXTMODE_CURSOR_DREG 0x3D5
#define TEXTMODE_ROWS 25
#define TEXTMODE_COLS 80
#define TEXTMODE_MEM_SIZE TEXTMODE_COLS + (TEXTMODE_ROWS*TEXTMODE_COLS)

#define IOCTL_TEXTMODE_COLOR         1
#define IOCTL_TEXTMODE_CURSER_POS    2
#define IOCTL_TEXTMODE_TOGGLE_CURSOR 3
#define IOCTL_TEXTMODE_CLEAR_MEM     4

static unsigned char _textmode_isopen = 0;
static unsigned char _textmode_xpos = 0;
static unsigned char _textmode_ypos = 0;
static unsigned char _textmode_show_cursor = 0;

static unsigned char _textmode_current_color = 0;
static unsigned short* _textmode_mem = (unsigned short*)0xB8000;

int textmode_drv_read(int* buffer, unsigned int count);
int textmode_drv_write(int* buffer, unsigned int count);
int textmode_drv_ioctl(unsigned int num, unsigned int param);
int textmode_drv_open();
void textmode_drv_close();

//Internal functions
void textmode_drv_update_cursor();

static driver_generic_t _textmode_driver =
{
    .read = textmode_drv_read,
    .write = textmode_drv_write,
    .ioctl = textmode_drv_ioctl,
    .open = textmode_drv_open,
    .close = textmode_drv_close
};

int textmode_driver_initialize()
{
    device_driver_t drv = {
        .name = "vga0",
        .type = DRV_GENERIC,
        .driver = &_textmode_driver
    };

    _textmode_isopen = 0;
    _textmode_show_cursor = 1;
    _textmode_current_color = 0x0F;

    printk("** Installing 80x25 textmode driver - ");
	return driver_register(&drv);
}

int textmode_drv_read(int* buffer, unsigned int count)
{
    int bytes_read = 0;
    unsigned int memloc = _textmode_xpos + (_textmode_ypos * TEXTMODE_COLS);

    for(; bytes_read<count; bytes_read++)
    {
        if(memloc < TEXTMODE_MEM_SIZE)
        {
            buffer[bytes_read] = (int)_textmode_mem[memloc];
            memloc++;
        }
        else { break; }
    }
    return bytes_read;
}

int textmode_drv_write(int* buffer, unsigned int count)
{
    int bytes_written = 0;
    for(; bytes_written<count; bytes_written++)
    {
        int data = buffer[bytes_written];
        int vgadata = (data & 0xFFFF);
        unsigned char x = (unsigned char)((data & 0xFF000000) >> 24);
        unsigned char y = (unsigned char)((data & 0x00FF0000) >> 16);

        if(x > TEXTMODE_COLS || y > TEXTMODE_ROWS)
        {
            printk("VGA ERROR: X %d, Y %d, vgadata %x, data %x\n", x, y, vgadata, data);
        }

        if(x < TEXTMODE_COLS && y < TEXTMODE_ROWS)
        {
            unsigned int memloc = x + (y * TEXTMODE_COLS);
            _textmode_mem[memloc] = (short)vgadata;
        }
    }
    return bytes_written;
}

int textmode_drv_ioctl(unsigned int ioctl_num, unsigned int param)
{
    switch (ioctl_num)
    {
        case IOCTL_TEXTMODE_COLOR:
            _textmode_current_color = (unsigned char)param;
        break;
        case IOCTL_TEXTMODE_TOGGLE_CURSOR:
        {
            if(param > 0)
            {
                _textmode_show_cursor = 1;
                textmode_drv_update_cursor();
            }
            else
            {
                _textmode_show_cursor = 0;
                //Just moving the cursor out of screen
                pio_outb(TEXTMODE_CURSOR_IREG, 0x0F);
                pio_outb(TEXTMODE_CURSOR_DREG, 0xFF);

                pio_outb(TEXTMODE_CURSOR_IREG, 0x0E);
                pio_outb(TEXTMODE_CURSOR_DREG, 0xFF);
            }
        } break;
        case IOCTL_TEXTMODE_CURSER_POS:
        {
            unsigned char x = (unsigned char)((param & 0xFF00) >> 8);
            unsigned char y = (unsigned char)(param & 0xFF);

            if(x < TEXTMODE_COLS && y < TEXTMODE_ROWS)
            {
                _textmode_xpos = x;
                _textmode_ypos = y;
            }
        } break;
        case IOCTL_TEXTMODE_CLEAR_MEM:
        {
            for (int i=0; i<(TEXTMODE_COLS*TEXTMODE_ROWS); i++)
            {
                _textmode_mem[i] =  (_textmode_current_color << 8) | 0x0020;
            }
            _textmode_xpos = 0;
            _textmode_ypos = 0;
        } break;
    }
    return TROS_DRIVER_OK;
}

int textmode_drv_open()
{
    if(!_textmode_isopen)
    {
        return TROS_DRIVER_OK;
    }
    else
    {
        return TROS_DRIVER_WASOPEN;
    }
}

void textmode_drv_close()
{
    _textmode_isopen = 0;
}

void textmode_drv_update_cursor()
{
    if(_textmode_show_cursor)
    {
        unsigned int mempos = (TEXTMODE_COLS * _textmode_ypos) + _textmode_xpos;
        unsigned char mempos_low = mempos & 0xFF;
        unsigned char mempos_high = (mempos >> 8) & 0xFF;

        pio_outb(TEXTMODE_CURSOR_IREG, 0x0F);
        pio_outb(TEXTMODE_CURSOR_DREG, mempos_low);

        pio_outb(TEXTMODE_CURSOR_IREG, 0x0E);
        pio_outb(TEXTMODE_CURSOR_DREG, mempos_high);
    }
}
