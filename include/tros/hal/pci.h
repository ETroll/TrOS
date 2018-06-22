// pci.h
// Basic PCI Methods


#ifndef INCLUDE_TROS_PCI_H
#define INCLUDE_TROS_PCI_H

#include <stdint.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC
#define PCI_CONFIG_NODEVICE 0xFFFF

#define PCI_WORD_VENDORID 0x00
#define PCI_WORD_DEVICEID 0x02
#define PCI_WORD_COMMAND 0x04
#define PCI_WORD_STATUS 0x06
#define PCI_BYTE_REVISIONID 0x08
#define PCI_BYTE_PROGIF 0x09
#define PCI_BYTE_SUBCLASS 0x0A
#define PCI_BYTE_CLASS 0x0B
#define PCI_BYTE_CHACHE_LINESIZE 0x0C
#define PCI_BYTE_LATENCY_TIMER 0x0D
#define PCI_BYTE_HEADERTYPE 0x0E
#define PCI_BYTE_BIST 0x0F
#define PCI_BYTE_INTERRUPT_LINE 0x3C
#define PCI_BYTE_INTERRUPT_PIN 0x3E

#define PCI_DWORD_BAR0 0x10
#define PCI_DWORD_BAR1 0x14
#define PCI_DWORD_BAR2 0x18
#define PCI_DWORD_BAR3 0x1C
#define PCI_DWORD_BAR4 0x20
#define PCI_DWORD_BAR5 0x24


#define PCI_HEADERTYPE_DEVICE 0x00
#define PCI_HEADERTYPE_BRIDGE 0x01
#define PCI_HEADERTYPE_CARDBUS 0x02
#define PCI_HEADERTYPE_MULTIPLE 0x80 //MF bit


uint8_t pci_read_byte(uint32_t bus, uint32_t slot, uint32_t func, uint32_t field);
uint16_t pci_read_word(uint32_t bus, uint32_t slot, uint32_t func, uint32_t field);
uint32_t pci_read_dword(uint32_t bus, uint32_t slot, uint32_t func, uint32_t field);

uint8_t pci_read_headertype(uint32_t bus, uint32_t slot, uint32_t func) ;

#endif