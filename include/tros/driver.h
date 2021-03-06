// driver.h
// Primitive driver system

#ifndef INCLUDE_TROS_DRIVER_H
#define INCLUDE_TROS_DRIVER_H

#define TROS_DRIVER_OK          1
#define TROS_DRIVER_WASOPEN     -2
#define TROS_DRIVER_ERROR       -1

typedef enum
{
    DRV_GENERIC = 0x01,
    DRV_BLOCK,
    DRV_FRAMEBUFFER
} driver_t;

typedef struct
{
    char name[10]; //TODO: change to char*
    driver_t type;
    void* driver;
    unsigned int id;
} device_driver_t;

typedef struct
{
    int (*read) (int* buffer, unsigned int count);
    int (*write) (int* buffer, unsigned int count);
    int (*ioctl) (unsigned int num, unsigned int param);
    int (*open) (void);
    void (*close) (void);
} driver_generic_t;

typedef struct
{
    void (*swapbuffer) (unsigned char* buffer, unsigned int length);
    int (*ioctl) (unsigned int num, unsigned int param);
    int (*open) (void);
    void (*close) (void);
} driver_framebuffer_t;

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
device_driver_t* driver_find_device(const char* name);
device_driver_t* driver_find_device_id(unsigned int id);

#endif
