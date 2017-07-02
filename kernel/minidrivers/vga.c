#include <tros/driver.h>
#include <tros/hal/io.h>
#include <tros/tros.h>


static uint8_t* _vga_mem_start = (uint8_t*)0xA0000;
static uint8_t* _vga_mem_end = (uint8_t*)0xBFFFF;

typedef enum
{
    VGA_IOCTL_SWAPBUFFER = 0x00,
    VGA_IOCTL_CHANGEMODE,
    VGA_IOCTL_GETMODES,
    VGA_IOCTL_INFOMODES
} vga_ioctl_t;

typedef enum
{
    VGA_DEPTH_256,
    VGA_DEPTH_16K
} vga_color_depth_t;

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
    vga_color_depth_t depth;
    vga_mode_regs_t regs;
} vga_mode_t;

// typedef enum
// {
//     VGA_MODE_640x480x16K = 0x00,
//     VGA_MODE_640x480x256,
//     VGA_MODE_600x400x16K,
//     VGA_MODE_600x400x256,
//     VGA_MODE_480x360x256,
//     VGA_MODE_320x240x256,
//     VGA_MODE_320x200x256
// } vga_modes_t;

static unsigned char _vga_isopen = 0;

#define VGA_NUM_MODES 2
static vga_mode_t modes[VGA_NUM_MODES];

//http://bos.asmhackers.net/docs/vga_without_bios/docs/mode%2013h%20without%20using%20bios.htm

//http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c

static int vga_read(int* buffer, unsigned int count);
static int vga_write(int* buffer, unsigned int count);
static int vga_ioctl(unsigned int num, unsigned int param);
static int vga_open();
static void vga_close();

static void vga_initmodes();
static void vga_changemode(vga_mode_t* mode);

static driver_generic_t _vga_driver =
{
    .read = vga_read,
    .write = vga_write,
    .ioctl = vga_ioctl,
    .open = vga_open,
    .close = vga_close
};

int vga_driver_initialize()
{
    device_driver_t drv = {
        .name = "vga1",
        .type = DRV_GENERIC,
        .driver = &_vga_driver
    };

    _vga_isopen = 0;
    vga_initmodes();

    printk("** Installing VGA driver - ");
	return driver_register(&drv);
}

#define R_COM  0x63 // "common" bits
#define R_W256 0x00
#define R_W320 0x00
#define R_W360 0x04
#define R_W376 0x04
#define R_W400 0x04

#define R_H200 0x00
#define R_H224 0x80
#define R_H240 0x80
#define R_H256 0x80
#define R_H270 0x80
#define R_H300 0x80
#define R_H360 0x00
#define R_H400 0x00
#define R_H480 0x80
#define R_H564 0x80
#define R_H600 0x80

#define SZ(x) (sizeof(x)/sizeof(x[0]))

int vga_open()
{
    if(!_vga_isopen)
    {
        _vga_isopen = 1;
        return TROS_DRIVER_OK;
    }
    else
    {
        return TROS_DRIVER_WASOPEN;
    }
}
void vga_close()
{
    _vga_isopen = 0;
}

int vga_read(int* buffer, unsigned int count)
{
    return 0;
}

int vga_write(int* buffer, unsigned int count)
{
    return 0;
}

int vga_ioctl(unsigned int num, unsigned int param)
{
    int retVal = 0;
    switch (num)
    {
        case VGA_IOCTL_SWAPBUFFER:
        {
        } break;
        case VGA_IOCTL_CHANGEMODE:
        {
            if(param < VGA_NUM_MODES)
            {
                vga_changemode(&modes[param]);
            }
        } break;
        case VGA_IOCTL_GETMODES:
        {
            //Return a basic list of modes
        } break;
        case VGA_IOCTL_INFOMODES:
        {
            //Get info on mode?
        } break;
    }
    return retVal;
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

    for (int i=0; i<(mode->width*mode->height); i++)
    {
        _vga_mem_start[i] =  0x0A;
    }
    pio_outb(0x3c0, 0x20); // enable video
}

void vga_initmodes()
{
    modes[0].width = 640;
    modes[0].height = 480;
    modes[0].depth = VGA_DEPTH_16K;
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
    modes[1].depth = VGA_DEPTH_256;
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
