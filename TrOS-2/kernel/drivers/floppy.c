// floppy.c
// Very generic driver for a single floppy disk drive based
// upon the Intel 8272A Floppy disk controller

#include <tros/driver.h>
#include <tros/irq.h>
#include <tros/hal/io.h>
#include <tros/tros.h>
#include <tros/scheduler.h> //temp sleep location

#define FLOPPY_IRQ 6
#define FDD_SECTORS_PER_TRACK 18

//FDD Controller IO ports
enum FDD_OI_PORT
{
    FDD_IO_DOR      = 0x3f2,
    FDD_IO_MSR      = 0x3f4,
    FDD_IO_FIFO     = 0x3f5,
    FDD_IO_CTRL     = 0x3f7
};

//Command nibble
enum FDD_COMMAND_NIBBLE
{
    FDD_CMD_READ_TRACK      = 2,
    FDD_CMD_SPECIFY         = 3,
    FDD_CMD_CHECK_STAT      = 4,
    FDD_CMD_WRITE_SECT      = 5,
    FDD_CMD_READ_SECT       = 6,
    FDD_CMD_CALIBRATE       = 7,
    FDD_CMD_CHECK_INT       = 8,
    FDD_CMD_FORMAT_TRACK    = 0xd,
    FDD_CMD_SEEK            = 0xf
};

//Command masks to be used with the command nibble
enum FDD_COMMAND_EXT
{
    FDD_CMD_EXT_SKIP        = 0x20,
    FDD_CMD_EXT_DENSITY     = 0x40,
    FDD_CMD_EXT_MULTITRACK  = 0x80
};

//Digital Output Register (DOR) masks
enum FDD_DOR_MASK
{
    FDD_DOR_MASK_DRIVE0         = 0,
    FDD_DOR_MASK_DRIVE1         = 1,
    FDD_DOR_MASK_DRIVE2         = 2,
    FDD_DOR_MASK_DRIVE3         = 3,
    FDD_DOR_MASK_RESET          = 4,
    FDD_DOR_MASK_DMA            = 8,
    FDD_DOR_MASK_DRIVE0_MOTOR   = 16,
    FDD_DOR_MASK_DRIVE1_MOTOR   = 32,
    FDD_DOR_MASK_DRIVE2_MOTOR   = 64,
    FDD_DOR_MASK_DRIVE3_MOTOR   = 128
};

//Main Status Register (MSR) Masks
enum FDD_MSR_MASK
{
    FDD_MSR_MASK_DRIVE1_POS_MODE    = 1,
    FDD_MSR_MASK_DRIVE2_POS_MODE    = 2,
    FDD_MSR_MASK_DRIVE3_POS_MODE    = 4,
    FDD_MSR_MASK_DRIVE4_POS_MODE    = 8,
    FDD_MSR_MASK_BUSY               = 16,
    FDD_MSR_MASK_DMA                = 32,
    FDD_MSR_MASK_DATAIO             = 64,
    FDD_MSR_MASK_DATAREG            = 128
};

//Controller Satus Port 0
enum FDD_ST0_MASK
{
    FDD_ST0_MASK_DRIVE0     = 0,
    FDD_ST0_MASK_DRIVE1     = 1,
    FDD_ST0_MASK_DRIVE2     = 2,
    FDD_ST0_MASK_DRIVE3     = 3,
    FDD_ST0_MASK_HEADACTIVE = 4,
    FDD_ST0_MASK_NOTREADY   = 8,
    FDD_ST0_MASK_UNITCHECK  = 16,
    FDD_ST0_MASK_SEEKEND    = 32,
    FDD_ST0_MASK_INTCODE    = 64
};

//Controller Status Port 0 - Codes
enum FDD_ST0_INTCODE_TYPE
{
    FDD_ST0_TYPE_NORMAL          = 0,
    FDD_ST0_TYPE_ABNORMAL_ERR    = 1,
    FDD_ST0_TYPE_INVALID_ERR     = 2,
    FDD_ST0_TYPE_NOTREADY        = 3
};

//GAP3 - Space between sectors on the physical disk
enum FDD_GAP3_LENGTH
{
    FDD_GAP3_LENGTH_STD    = 42,
    FDD_GAP3_LENGTH_5_14   = 32,
    FDD_GAP3_LENGTH_3_5    = 27
};

//Bytes per sector. (DTL)
//Formula: 2^sector_number * 128, where ^ denotes "to the power of"
enum FDD_SECTOR_DTL
{
    FDD_SECTOR_DTL_128  = 0,
    FDD_SECTOR_DTL_256  = 1,
    FDD_SECTOR_DTL_512  = 2,
    FDD_SECTOR_DTL_1024 = 4
};

typedef struct
{
    int head;
    int track;
    int sector;
} chs_t;

void fdd_irq_handler(cpu_registers_t* regs);

// Temp DMA functions before a generic DMA driver have been created
void fdd_dma_init();
void fdd_dma_read();
void fdd_dma_write();


// Driver interface functions
int floppy_read(unsigned char *buffer, unsigned int sector);
int floppy_write(unsigned char *data, unsigned int sector);
void floppy_format(unsigned int track, unsigned int head);
int floppy_seek(unsigned int track, unsigned int head);
int floppy_open();
void floppy_close();

// Internal functions used by the interface functions
static void fdd_check_interrupt_status(unsigned int* status_reg0, unsigned int* cur_cylinder);
static void fdd_send_command(unsigned char cmd);
static void fdd_motor_on();
static void fdd_motor_off();
static void fdd_wait_irq();
static unsigned char fdd_read_data();
static void fdd_reset();
static int fdd_calibrate();
static void fdd_send_drivedata(unsigned int steprate, unsigned int load_time,
    unsigned int  unload_time, unsigned char dma);
static chs_t fdd_lba_to_chs(int lba);

static volatile unsigned char __fdd_irq_fired = 0;
static unsigned char __fdd_current_drive = 0;

static driver_block_t __fdd_driver = {
    .read = floppy_read,
    .write = floppy_write,
    .format = floppy_format,
    .seek = floppy_seek,
    .open = floppy_open,
    .close = floppy_close
};

int floppy_driver_initialize(unsigned char device)
{
    device_driver_t drv = {
        .name = "fdd",
        .type = DRV_BLOCK,
        .driver = &__fdd_driver
    };
    if(device < 4)
    {
        __fdd_current_drive = device;
        printk("** Initalizing generic floppy disk driver\n");

        return driver_register(&drv);
    }
    else
    {
        return -1;
    }
}

int floppy_open()
{
    __fdd_irq_fired = 0;

    if(irq_register_handler(FLOPPY_IRQ + IRQ_BASE, &fdd_irq_handler))
    {
        fdd_dma_init();
        fdd_reset();

        //steprate=13ms, load_time=1ms unload_time=15ms, DMA=ON
        //fdd_send_drivedata(13, 1, 15, 1);   //NOTE: Is this needed?

        return 1;
    }
    else
    {
        return 0;
    }
}

void floppy_close()
{
    irq_remove_handler(FLOPPY_IRQ + IRQ_BASE);
}

void fdd_irq_handler(cpu_registers_t* regs)
{
    __fdd_irq_fired = 1;

    irq_eoi(FLOPPY_IRQ);
}

//Sector comes as a Logical Block Address
int floppy_read(unsigned char *buffer, unsigned int sector)
{
    //TODO: Sett DMA to point to buffer addr

    chs_t chs = fdd_lba_to_chs(sector);
    fdd_motor_on();
    printk("CHS: %d / %d / %d\n", chs.track, chs.head, chs.sector);

    if(floppy_seek(chs.track, chs.head))
    {
        //read a block
        unsigned int status = 0;
        unsigned int cylinder = 0;
        unsigned char result[7];

        fdd_dma_read();

        fdd_send_command(FDD_CMD_READ_SECT
            | FDD_CMD_EXT_MULTITRACK
            | FDD_CMD_EXT_SKIP
            | FDD_CMD_EXT_DENSITY);

        fdd_send_command(chs.head << 2 | __fdd_current_drive );
        fdd_send_command(chs.track);
        fdd_send_command(chs.head);
        fdd_send_command(chs.sector);
        fdd_send_command(FDD_SECTOR_DTL_512);
        fdd_send_command(((sector + 1) >= FDD_SECTORS_PER_TRACK) ?
            FDD_SECTORS_PER_TRACK :
            sector + 1);
        fdd_send_command(FDD_GAP3_LENGTH_3_5); //NOTE: Can change! Depends on device
        fdd_send_command(0xFF);

        fdd_wait_irq();

        for(int i=0; i<7; i++)
        {
            result[i] = fdd_read_data();
        }

        fdd_check_interrupt_status(&status, &cylinder);

        fdd_motor_off();

        // unsigned char* tmp = (unsigned char*)DMA_BUFFER;
        // for(int i = 0; i<512; i++)
        // {
        //     buffer[i] = tmp[i];
        // }

        //printk("%d - %d\n", chs.sector, result[5]);
        return (int)(chs.sector- result[5]); //number of sectors read
    }
    else
    {
        return 0;
    }
}

int floppy_write(unsigned char *data, unsigned int sector)
{
    return 0;
}

void floppy_format(unsigned int track, unsigned int head)
{

}

int floppy_seek(unsigned int track, unsigned int head)
{
    unsigned int status = 0;
    unsigned int cylinder = 0; //Same as a track...

    for(int i = 0; i<10; i++)
    {
        fdd_send_command(FDD_CMD_SEEK);
        fdd_send_command((head << 2) | __fdd_current_drive);
        fdd_send_command(track);

        fdd_wait_irq();
        fdd_check_interrupt_status(&status, &cylinder);

        if(cylinder == track)
        {
            return 1;
        }
    }

    return 0;
}
void fdd_dma_init()
{
    pio_outb(0x06, 0x0a);
    pio_outb(0xff, 0xd8);
    pio_outb(0x00, 0x04);
    pio_outb(0x10, 0x04);
    pio_outb(0xff, 0xd8);
    pio_outb(0xff, 0x05);
    pio_outb(0x23, 0x05);
    pio_outb(0x00, 0x80);
    pio_outb(0x02, 0x0a);
}

void fdd_dma_read()
{
    pio_outb(0x06, 0x0a);
    pio_outb(0x56, 0x0b);
    pio_outb(0x02, 0x0a);
}

void fdd_dma_write()
{
    pio_outb(0x06, 0x0a);
    pio_outb(0x5a, 0x0b);
    pio_outb(0x02, 0x0a);
}

static chs_t fdd_lba_to_chs(int lba)
{
    chs_t chs = {
        .head = (lba % (FDD_SECTORS_PER_TRACK * 2)) / (FDD_SECTORS_PER_TRACK),
        .track = lba / (FDD_SECTORS_PER_TRACK * 2),
        .sector = lba % FDD_SECTORS_PER_TRACK + 1
    };
    return chs;
}

static void fdd_wait_irq()
{
    //TODO: Maybe implement SIGNALS to replace this shit?
    while(__fdd_irq_fired == 0)
    {
        //printk(".");
    }
    __fdd_irq_fired = 0;
}

static void fdd_send_drivedata(unsigned int steprate, unsigned int load_time,
    unsigned int  unload_time, unsigned char dma)
{
    unsigned int tmp = 0;
    fdd_send_command(FDD_CMD_SPECIFY);

    tmp = ((steprate & 0x0F) << 4) | (unload_time & 0x04);
    fdd_send_command(tmp);

    tmp = (load_time << 1) | (dma ? 0 : 1);
    fdd_send_command(tmp);
}

static void fdd_send_command(unsigned char cmd)
{
    //Very primitive "timeout" functionality :P
    //NOTE: This does not contain error handling if the register never gets
    //      ready. Keep in mind! Stop beeing a slacker and fix this..
    for (int i = 0; i < 500; i++ )
    {
        if(pio_inb(FDD_IO_MSR) & FDD_MSR_MASK_DATAREG)
        {
            pio_outb(cmd, FDD_IO_FIFO);
            break;
        }
    }
}

static unsigned char fdd_read_data()
{
    unsigned char data = 0;
    for (int i = 0; i < 500; i++ )
    {
        if(pio_inb(FDD_IO_MSR) & FDD_MSR_MASK_DATAREG)
        {
            data = pio_inb(FDD_IO_FIFO);
            break;
        }
    }
    return data;
}

static void fdd_motor_on()
{
    unsigned int motor = 0;

    switch (__fdd_current_drive)
    {
        case 0:
            motor = FDD_DOR_MASK_DRIVE0_MOTOR;
            break;
        case 1:
            motor = FDD_DOR_MASK_DRIVE1_MOTOR;
            break;
        case 2:
            motor = FDD_DOR_MASK_DRIVE2_MOTOR;
            break;
        case 3:
            motor = FDD_DOR_MASK_DRIVE3_MOTOR;
            break;
    }

    pio_outb(__fdd_current_drive
        | motor
        | FDD_DOR_MASK_RESET
        | FDD_DOR_MASK_DMA, FDD_IO_DOR);
	scheduler_sleep(5);
}

static void fdd_motor_off()
{
    pio_outb(FDD_DOR_MASK_RESET, FDD_IO_DOR);
	scheduler_sleep(5);
}

static void fdd_check_interrupt_status(unsigned int* status_reg0, unsigned int* cur_cylinder)
{
    fdd_send_command(FDD_CMD_CHECK_INT);
    *status_reg0 = fdd_read_data();
    *cur_cylinder = fdd_read_data();
}

static int fdd_calibrate()
{
    unsigned int status = 0;
    unsigned int cylinder = 0;

    fdd_motor_on();

    for(int i = 0; i<10; i++)
    {
        fdd_send_command(FDD_CMD_CALIBRATE);
        fdd_send_command(__fdd_current_drive);
        fdd_wait_irq();

        fdd_check_interrupt_status(&status, &cylinder);

        if(cylinder == 0)
        {
            fdd_motor_off();
            return 1;
        }
    }
    //It failed! Oh noes!
    fdd_motor_off();
    return 0;
}

static void fdd_reset()
{
    unsigned int status = 0;
    unsigned int cylinder = 0;
    pio_outb(0, FDD_IO_DOR);
    pio_outb(FDD_DOR_MASK_RESET | FDD_DOR_MASK_DMA, FDD_IO_DOR);
    fdd_wait_irq();


    for(int i=0; i<4; i++)
    {
        fdd_check_interrupt_status(&status, &cylinder);
    }
    pio_outb(0, FDD_IO_CTRL); //Sets transfer speed to 500Kb/s

    //steprate=3ms, load_time=16ms unload_time=240ms, DMA=ON
    fdd_send_drivedata(3, 16, 240, 1);
    fdd_calibrate();
}
