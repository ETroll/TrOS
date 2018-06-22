// hwdetect.h
// Hardware detections

#ifndef INCLUDE_TROS_HWDETECT_H
#define INCLUDE_TROS_HWDETECT_H

#include <tros/klib/list.h>

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

// typedef struct 
// {

// } hwd_device_t;

hwd_floppy_t hwdetect_floppy_disks();
void hwdetect_enumerate_hardware();

#endif
