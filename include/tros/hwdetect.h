// hwdetect.h
// Hardware detections

#ifndef INCLUDE_TROS_HWDETECT_H
#define INCLUDE_TROS_HWDETECT_H

#include <tros/klib/list.h>
#include <tros/driver.h>
#include <tros/tros.h>
#include <stdint.h>

typedef enum
{
    HWD_FDD_NONE        = 0,
    HWD_FDD_360_525     = 1,
    HWD_FDD_1200_525    = 2,
    HWD_FDD_720_35      = 3,
    HWD_FDD_1440_35     = 4,
    HWD_FDD_2880_35     = 5
} hwd_floppy_device_t;

typedef struct
{
    hwd_floppy_device_t master;
    char* master_desc;
    hwd_floppy_device_t slave;
    char* slave_desc;
} hwd_floppy_t;

typedef enum 
{
    HWD_PCI = 0,
    HWD_USB,
    HWD_ISA,
    HWD_PLATFORM, //PORT
    HWD_MEMORYMAPPED
} hwd_type_t;

typedef struct hwd_device
{
    uint32_t vendorid;
    uint32_t deviceid;
    uint32_t class;
    uint32_t subclass;
    uint32_t progif;
    uint32_t irq;
    hwd_type_t* type;
} hwd_device_t;

hwd_floppy_t hwdetect_floppy_disks();
tros_status_t hwdetect_enumerate_hardware(list_t* hwlist);

#endif
