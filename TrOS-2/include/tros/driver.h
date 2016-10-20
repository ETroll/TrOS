// driver.h
// Primitive driver system

#ifndef INCLUDE_TROS_DRIVER_H
#define INCLUDE_TROS_DRIVER_H

#define TROS_DRIVER_OK          1
#define TROS_DRIVER_WASOPEN     -2
#define TROS_DRIVER_ERROR       -1

typedef enum
{
    DRV_CHAR = 0x01,
    DRV_BLOCK,
    DRV_HID
} driver_t;

// typedef struct
// {
//     int (*open) (void);
//     void (*close) (void);
//     int (*ioctl) (unsigned int num, unsigned long param);
// } dops_t;   //Device operations shared accross all devices

typedef struct
{
    char name[10]; //TODO: change to char*
    driver_t type;
    void* driver;
    unsigned int id;

} device_driver_t;

typedef struct
{
    int (*read) (int *buffer, unsigned int count);
    int (*ioctl) (unsigned int num, unsigned int param);
    int (*open) (void);
    void (*close) (void);
} driver_hid_t;

typedef struct
{
    int (*read) (char *buffer, unsigned int count);
    int (*write) (char *buffer, unsigned int count);
    int (*seek) (unsigned int pos);
    int (*ioctl) (unsigned int num, unsigned int param);
    int (*open) (void);
    void (*close) (void);
} driver_char_t;

typedef struct
{
    int (*read) (unsigned char *buffer, unsigned int block, unsigned nblocks);
    int (*write) (unsigned char *data, unsigned int sector);
    void (*format) (unsigned int track, unsigned int head);
    int (*seek) (unsigned int track, unsigned int head);
    int (*open) (void);
    void (*close) (void);
} driver_block_t;

//Set up the driver system
void driver_initialize();
int driver_register(device_driver_t* driver);
device_driver_t* driver_find_device(char* name);
device_driver_t* driver_find_device_id(unsigned int id);

#endif
