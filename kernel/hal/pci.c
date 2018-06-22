#include <tros/hal/pci.h>
#include <tros/hal/io.h>

uint32_t pci_address(uint32_t bus, uint32_t slot, uint32_t func, uint32_t field)
{
    return (uint32_t)((bus << 16) | (slot << 11) | 
        (func << 8) | (field & PCI_CONFIG_DATA) | ((uint32_t)0x80000000));
}

uint8_t pci_read_byte(uint32_t bus, uint32_t slot, uint32_t func, uint32_t field)
{
    pio_outdw(PCI_CONFIG_ADDRESS, pci_address(bus, slot, func, field));
    return pio_inb(PCI_CONFIG_DATA + (field & 3));
}

uint16_t pci_read_word(uint32_t bus, uint32_t slot, uint32_t func, uint32_t field)
{
    pio_outdw(PCI_CONFIG_ADDRESS, pci_address(bus, slot, func, field));
    /* read in the data */
    /* (field & 2) * 8) = 0 will choose the first word of the 32 bits register */
    // tmp = (uint16_t)((sysInLong (0xCFC) >> ((field & 2) * 8)) & 0xffff);
    uint16_t retval = pio_inw(PCI_CONFIG_DATA + (field & 2));
    // uint16_t retval = pio_indw(PCI_CONFIG_DATA >> ((field & 2)*8)) & 0xffff;
    return retval;
}

uint32_t pci_read_dword(uint32_t bus, uint32_t slot, uint32_t func, uint32_t field)
{
    pio_outdw(PCI_CONFIG_ADDRESS, pci_address(bus, slot, func, field));
    return pio_indw(PCI_CONFIG_DATA);
}

uint8_t pci_read_headertype(uint32_t bus, uint32_t slot, uint32_t func) 
{
    return pci_read_byte(bus, slot, func, PCI_BYTE_HEADERTYPE);
}