// hwdetect.c
// Generic hardware detection routines

#include <tros/hwdetect.h>
#include <tros/hal/io.h>
#include <tros/hal/pci.h>

#include <tros/tros.h> //prink -- todo remove!

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
    pio_outb(0x70, 0x10);
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

void hwdetect_pci_device(uint8_t bus, uint8_t slot) 
{
    for(uint8_t function = 0; function < 8; function++) 
    {
        uint16_t vendorid = pci_read_word(bus, slot, function, PCI_WORD_VENDORID);
        if(vendorid != PCI_CONFIG_NODEVICE) 
        {
            printk("PCI: Found device (%d, %d, %d)\n", bus, slot, function);

            uint16_t deviceid = pci_read_word(bus, slot, function, PCI_WORD_DEVICEID);
            uint8_t baseclass = pci_read_byte(bus, slot, function, PCI_BYTE_CLASS);
            uint8_t subclass = pci_read_byte(bus, slot, function, PCI_BYTE_SUBCLASS);
            uint8_t progif = pci_read_byte(bus, slot, function, PCI_BYTE_PROGIF);
            uint8_t irqline = pci_read_byte(bus, slot, function, PCI_BYTE_INTERRUPT_LINE);

            printk("     VendorID: %x - DeviceID: %x\n", vendorid, deviceid);
            printk("     Baseclass: %x - Subclass: %x\n", baseclass, subclass);
            printk("     ProgIf: %x - IRQ Line: %x\n", progif, irqline);

            printk("PCI: -- DONE --\n");
        }
        else 
        {
            break;
        }
    }
 }

 void hwdetect_pci_checkbus(list_t* hwlist, uint8_t bus) 
 {
     for(uint8_t slot = 0; slot < 32; slot++) 
     {
         hwdetect_pci_device(bus, slot);
     }
 }

//return a list of devices. PCI, USB, MEMORYMAPPED, PORT (etc)
tros_status_t hwdetect_enumerate_hardware(list_t* hwlist)
{
    uint8_t header = pci_read_headertype(0, 0, 0);
    if( (header & PCI_HEADERTYPE_MULTIPLE) == 0) 
    {
        //Single PCI host controller
        printk("PCI: Single bus detected\n");
        hwdetect_pci_checkbus(hwlist, 0);
    }
    else 
    {
        //Multiple PCI host controllers
        printk("PCI: Multiple bus's detected\n");
        for(uint8_t function = 0; function < 8; function++) 
        {
            if(pci_read_word(0, 0, function, PCI_WORD_VENDORID) != 0xFFFF) 
            {
                hwdetect_pci_checkbus(hwlist, function);        
            }
            else 
            {
                break;
            }
        }
    }
}