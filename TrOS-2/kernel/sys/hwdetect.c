// hwdetect.c
// Generic hardware detection routines

#include <tros/hwdetect.h>
#include <tros/hal/io.h>

char *floppy_descripive_names[6] =
{
    "No floppy drive",
    "360kb 5.25in",
    "1.2mb 5.25in",
    "720kb 3.5in",
    "1.44mb 3.5in",
    "2.88mb 3.5in"
};

hwd_floppy_t hwdetect_floppy_disks()
{
    pio_outb(0x10, 0x70);
    unsigned char data = pio_inb(0x71);

    //High nibble -> Master device
    //Low nibble -> Slave device

    hwd_floppy_t info =
    {
        .master = data >> 4,
        .master_desc = floppy_descripive_names[data >> 4],
        .slave = data & 0x0F,
        .slave_desc = floppy_descripive_names[data & 0x0F]
    };

    return info;
}
