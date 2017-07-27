#include <tros/driver.h>
#include <tros/hal/io.h>
#include <tros/tros.h>
#include <tros/memory.h>

#define VGA_MEM_SIZE 65535

typedef struct
{
    uint8_t unused:7;
    uint8_t dirty:1;
    uint8_t data[4];
} __attribute__((packed)) vga_frame_t; // maybe name vga_mem_frame?

typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t bpp;       //if zero, then text-mode
    uint32_t mode;
} fbmode_information_t;

typedef enum
{
    VGA_IOCTL_CHANGEMODE = 0x00,
    VGA_IOCTL_GETMODES
} vga_ioctl_t;

typedef enum
{
    VGA_MEMORY_LINEAR,
    VGA_MEMORY_PLANAR
} vga_memory_model_t;

typedef struct
{
    uint8_t modeControl;
    uint8_t overscan;
    uint8_t colorPlaneEnable;
    uint8_t horizontalPanning;
    uint8_t colorSelect;
    uint8_t miscOutput;
    uint16_t clockMode;
    uint16_t characterSelect;
    uint16_t memoryMode;
    uint16_t mode;
    uint16_t misc;
    uint16_t horizontalTotal;
    uint16_t horizontalDisplayEnableEnd;
    uint16_t horizontalBlankStart;
    uint16_t horizontalBlankEnd;
    uint16_t horizontalRetraceStart;
    uint16_t horizontalRetraceEnd;
    uint16_t verticalTotal;
    uint16_t overflow;
    uint16_t presetRowScan;
    uint16_t maximumScanLine;
    uint16_t verticalRetraceStart;
    uint16_t verticalRetraceEnd;
    uint16_t verticalDisplayEnableEnd;
    uint16_t logicalWidth;
    uint16_t underlineLocation;
    uint16_t verticalBlankStart;
    uint16_t verticalBlankEnd;
    uint16_t modeControl2;
} vga_mode_regs_t;

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    vga_mode_regs_t regs;
    vga_memory_model_t mmodel;
} vga_mode_t;


static uint8_t* _vga_mem_start = (uint8_t*)0xA0000;
static uint8_t* _vga_mem_end = (uint8_t*)0xBFFFF;

static unsigned char _vga_isopen = 0;

#define VGA_NUM_MODES 2
static vga_mode_t modes[VGA_NUM_MODES];
static vga_mode_t* _active_mode = 0;
static vga_frame_t* _buffer = 0;

//http://bos.asmhackers.net/docs/vga_without_bios/docs/mode%2013h%20without%20using%20bios.htm

//http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c

static void vga_swapbuffer(unsigned char* buffer, unsigned int length);
static int vga_ioctl(unsigned int num, unsigned int param);
static int vga_open();
static void vga_close();

static void vga_initmodes();
static void vga_changemode(vga_mode_t* mode);
static void vga_drawbuffer();

static driver_framebuffer_t _vga_driver =
{
    .swapbuffer = vga_swapbuffer,
    .ioctl = vga_ioctl,
    .open = vga_open,
    .close = vga_close
};

int vga_driver_initialize()
{
    device_driver_t drv = {
        .name = "vga1",
        .type = DRV_FRAMEBUFFER,
        .driver = &_vga_driver
    };

    _vga_isopen = 0;
    _active_mode = 0;
    vga_initmodes();

    printk("** Installing VGA driver - ");
	return driver_register(&drv);
}

int vga_open()
{
    if(!_vga_isopen)
    {
        _vga_isopen = 1;
        _buffer = (vga_frame_t*)kmalloc(sizeof(vga_frame_t)*VGA_MEM_SIZE);
        for(uint32_t i = 0; i<VGA_MEM_SIZE; i++)
        {
            //Maybe clear mem instead? Bit slower, but safer!
            _buffer[i].dirty = 0;
        }
        return TROS_DRIVER_OK;
    }
    else
    {
        return TROS_DRIVER_WASOPEN;
    }
}
void vga_close()
{
    if(_buffer)
    {
        kfree(_buffer);
    }
    _vga_isopen = 0;
}

void vga_swapbuffer(unsigned char* buffer, unsigned int length)
{

}

int vga_ioctl(unsigned int num, unsigned int param)
{
    int retVal = 0;
    static int nextmode = 0;
    printk("VGA IOCTL %d - PARAM: %x\n", num, param);
    switch (num)
    {
        case VGA_IOCTL_CHANGEMODE:
        {
            if(param < VGA_NUM_MODES)
            {
                vga_changemode(&modes[param]);
                retVal = 1;
            }
        } break;
        case VGA_IOCTL_GETMODES:
        {
            if(nextmode < VGA_NUM_MODES)
            {
                fbmode_information_t* fbinfo = (fbmode_information_t*)param;
                fbinfo->width = modes[nextmode].width;
                fbinfo->height = modes[nextmode].height;
                fbinfo->bpp = modes[nextmode].bpp;
                fbinfo->mode = nextmode;
                retVal = ++nextmode;
                printk("Retrning info: %dx%dx%d\n", fbinfo->width, fbinfo->height, fbinfo->bpp);
            }
            else
            {
                printk("Resetting counter\n");
                nextmode = 0;
            }
        } break;
    }
    printk("Retval: %d\n", retVal);
    return retVal;
}

void vga_set_plane(uint32_t plane)
{
    plane &= 3; //Just in case, clear rest of the int
    pio_outb(0x3C4, 2);
    pio_outb(0x3C5, 1 << plane);
}

void vga_drawbuffer()
{
    if(_buffer)
    {
        for(uint32_t i = 0; i<VGA_MEM_SIZE; i++)
        {
            if(_buffer[i].dirty)
            {
                if(_active_mode->mmodel == VGA_MEMORY_PLANAR)
                {
                    vga_set_plane(0);
                    _vga_mem_start[i] = _buffer[i].data[0];
                    vga_set_plane(1);
                    _vga_mem_start[i] = _buffer[i].data[1];
                    vga_set_plane(2);
                    _vga_mem_start[i] = _buffer[i].data[2];
                    vga_set_plane(3);
                    _vga_mem_start[i] = _buffer[i].data[3];
                }
                else
                {
                    _vga_mem_start[i] = _buffer[i].data[0];
                }
            }
        }
    }
}

void vga_write_pixel_linear(uint32_t x, uint32_t y, uint8_t c)
{
    if(_buffer)
    {
        //320y = 256y + 64y,
        //        2^8 + 2^6
        //uint16_t offset = _active_mode->width * y + x;
        uint16_t offset = offset = (y<<8) + (y<<6) + x;
        _buffer[offset].data[0] = c;
        _buffer[offset].dirty = 1;
    }
}

void vga_write_pixel_planar(uint32_t x, uint32_t y, uint8_t c)
{
    //640 = 512 + 128 = 128 × 5, and 480 = 32 × 15.
    //      2^16 + 2^7
    // uint16_t offset = offset = (y<<14) + (y<<5) + (x>>2);
    if(_buffer)
    {
        uint32_t offset = (y*_active_mode->width) / 8 + (x/8);
        x = 7-(x%8);

        _buffer[offset].dirty = 1;

        uint8_t bitmask = 1 << x;
        uint8_t planemask = 1;
        for(uint8_t p = 0; p < 4; p++)
        {
            if(planemask & c)
            {
                _buffer[offset].data[p] |= bitmask;
            }
            else
            {
                _buffer[offset].data[p] &= ~bitmask;
            }
            planemask <<= 1;
        }
    }
}

void vga_test_linear(uint32_t width, uint32_t height)
{
    for(uint32_t i = 0; i<VGA_MEM_SIZE; i++)
    {
        _buffer[i].dirty = 1;
        _buffer[i].data[0] = 0x00;
    }

    for(int x=50; x<150; x++)
    {
        for(int y=50; y<150; y++)
        {
            vga_write_pixel_linear(x,y, 0xF);
        }
    }
    vga_drawbuffer();
}

void vga_test_planar(uint32_t width, uint32_t height)
{
    for(uint32_t i = 0; i<VGA_MEM_SIZE; i++)
    {
        _buffer[i].dirty = 1;
        _buffer[i].data[0] = 0xFF;
        _buffer[i].data[1] = 0xFF;
        _buffer[i].data[2] = 0xFF;
        _buffer[i].data[3] = 0xFF;
    }

    for(int x=0; x<640; x++)
    {
        for(int y=0; y<480; y++)
        {
            if(x % 2 == 0)
            {
                if(y % 2 == 0)
                {
                    vga_write_pixel_planar(x,y, 0x01);
                }
            }
            else
            {
                if(y % 2 == 1)
                {
                    vga_write_pixel_planar(x,y, 0x07);
                }
            }
        }
    }

    for(int x=100; x<200; x++)
    {
        for(int y=100; y<200; y++)
        {
            vga_write_pixel_planar(x,y, 0x02);
        }
    }

    for(int x=300; x<400; x++)
    {
        for(int y=200; y<300; y++)
        {
            if(y == 200 || y == 299)
            {
                vga_write_pixel_planar(x,y, 0x00);
            }
            else if(x==300 || x ==399)
            {
                vga_write_pixel_planar(x,y, 0x00);
            }
        }
    }



    vga_drawbuffer();
}

void vga_changemode(vga_mode_t* mode)
{

    pio_outb(0x3c2, mode->regs.miscOutput);
    pio_outw(0x3d4, mode->regs.verticalRetraceEnd);

    pio_outw(0x3d4, mode->regs.horizontalTotal);
    pio_outw(0x3d4, mode->regs.horizontalDisplayEnableEnd);
    pio_outw(0x3d4, mode->regs.horizontalBlankStart);
    pio_outw(0x3d4, mode->regs.horizontalBlankEnd);
    pio_outw(0x3d4, mode->regs.horizontalRetraceStart);
    pio_outw(0x3d4, mode->regs.horizontalRetraceEnd);
    pio_outw(0x3d4, mode->regs.verticalRetraceEnd);
    pio_outw(0x3d4, mode->regs.logicalWidth);

    pio_outw(0x3d4, mode->regs.verticalTotal);
    pio_outw(0x3d4, mode->regs.overflow);
    pio_outw(0x3d4, mode->regs.maximumScanLine);
    pio_outw(0x3d4, mode->regs.verticalRetraceStart);
    pio_outw(0x3d4, mode->regs.verticalRetraceEnd);
    pio_outw(0x3d4, mode->regs.verticalDisplayEnableEnd);
    pio_outw(0x3d4, mode->regs.verticalBlankStart);
    pio_outw(0x3d4, mode->regs.verticalBlankEnd);

    pio_outw(0x3d4, mode->regs.presetRowScan);
    pio_outw(0x3d4, mode->regs.underlineLocation);
    pio_outw(0x3d4, mode->regs.modeControl2);

    pio_outw(0x3c4, mode->regs.clockMode);
    pio_outw(0x3c4, mode->regs.characterSelect);
    pio_outw(0x3c4, mode->regs.memoryMode);
    pio_outw(0x3c4, 0x0f02); // enable writing to all planes


    pio_outw(0x3ce, mode->regs.mode);
    pio_outw(0x3ce, mode->regs.misc);

    pio_inb(0x3da);
    pio_outb(0x3c0, 0x10);
    pio_outb(0x3c0, mode->regs.modeControl);
    pio_outb(0x3c0, 0x11);
    pio_outb(0x3c0, mode->regs.overscan);
    pio_outb(0x3c0, 0x12);
    pio_outb(0x3c0, mode->regs.colorPlaneEnable);
    pio_outb(0x3c0, 0x13);
    pio_outb(0x3c0, mode->regs.horizontalPanning);
    pio_outb(0x3c0, 0x14);
    pio_outb(0x3c0, mode->regs.colorSelect);

    _active_mode = mode;
    if((mode->width * mode->height) > 65536)
    {
        printk("Testing planar!\n");
        vga_test_planar(mode->width, mode->height);

    }
    else
    {
        printk("Testing linear!\n");
        vga_test_linear(mode->width, mode->height);
    }

    pio_outb(0x3c0, 0x20); // enable video


}

void vga_initmodes()
{
    modes[0].width = 640;
    modes[0].height = 480;
    modes[0].bpp = 4;
    modes[0].mmodel = VGA_MEMORY_PLANAR;
    modes[0].regs.modeControl = 0x01;
    modes[0].regs.overscan = 0x00;
    modes[0].regs.colorPlaneEnable = 0x0F;
    modes[0].regs.horizontalPanning = 0x00;
    modes[0].regs.colorSelect = 0x00;
    modes[0].regs.miscOutput = 0xE3;
    modes[0].regs.clockMode = 0x0101;
    modes[0].regs.characterSelect = 0x0003;
    modes[0].regs.memoryMode = 0x0204;
    modes[0].regs.mode = 0x0005;
    modes[0].regs.misc = 0x0506;
    modes[0].regs.horizontalTotal = 0x5F00;
    modes[0].regs.horizontalDisplayEnableEnd = 0x4F01;
    modes[0].regs.horizontalBlankStart = 0x5002;
    modes[0].regs.horizontalBlankEnd = 0x8203;
    modes[0].regs.horizontalRetraceStart = 0x5404;
    modes[0].regs.horizontalRetraceEnd = 0x8005;
    modes[0].regs.verticalTotal = 0x0B06;
    modes[0].regs.overflow = 0x3E07;
    modes[0].regs.presetRowScan = 0x0008;
    modes[0].regs.maximumScanLine = 0x4009;
    modes[0].regs.verticalRetraceStart = 0xEA10;
    modes[0].regs.verticalRetraceEnd = 0x8C11;
    modes[0].regs.verticalDisplayEnableEnd = 0xDF12;
    modes[0].regs.logicalWidth = 0x2813;
    modes[0].regs.underlineLocation = 0x0014;
    modes[0].regs.verticalBlankStart = 0xE715;
    modes[0].regs.verticalBlankEnd = 0x0416;
    modes[0].regs.modeControl2 = 0xE317;

    modes[1].width = 320;
    modes[1].height = 200;
    modes[1].bpp = 8;
    modes[1].mmodel = VGA_MEMORY_LINEAR;
    modes[1].regs.modeControl = 0x41;
    modes[1].regs.overscan = 0x00;
    modes[1].regs.colorPlaneEnable = 0x0F;
    modes[1].regs.horizontalPanning = 0x00;
    modes[1].regs.colorSelect = 0x00;
    modes[1].regs.miscOutput = 0x63;
    modes[1].regs.clockMode = 0x0101;
    modes[1].regs.characterSelect = 0x0003;
    modes[1].regs.memoryMode = 0x0E04;
    modes[1].regs.mode = 0x4005;
    modes[1].regs.misc = 0x0506;
    modes[1].regs.horizontalTotal = 0x5F00;
    modes[1].regs.horizontalDisplayEnableEnd = 0x4F01;
    modes[1].regs.horizontalBlankStart = 0x5002;
    modes[1].regs.horizontalBlankEnd = 0x8203;
    modes[1].regs.horizontalRetraceStart = 0x5404;
    modes[1].regs.horizontalRetraceEnd = 0x8005;
    modes[1].regs.verticalTotal = 0xBF06;
    modes[1].regs.overflow = 0x1F07;
    modes[1].regs.presetRowScan = 0x0008;
    modes[1].regs.maximumScanLine = 0x4109;
    modes[1].regs.verticalRetraceStart = 0x9C10;
    modes[1].regs.verticalRetraceEnd = 0x8E11;
    modes[1].regs.verticalDisplayEnableEnd = 0x8F12;
    modes[1].regs.logicalWidth = 0x2813;
    modes[1].regs.underlineLocation = 0x4014;
    modes[1].regs.verticalBlankStart = 0x9615;
    modes[1].regs.verticalBlankEnd = 0xB916;
    modes[1].regs.modeControl2 = 0xA317;
}
