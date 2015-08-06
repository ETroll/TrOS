// driver.h
// Primitive driver system

#ifndef INCLUDE_TROS_DRIVER_H
#define INCLUDE_TROS_DRIVER_H

#define TROS_DRIVER_OK          1
#define TROS_DRIVER_WASOPEN     0
#define TROS_DRIVER_ERROR       -1

typedef enum
{
    DRV_NONE,
    DRV_CHAR,
    DRV_BLOCK,
    DRV_HID
} driver_t;

typedef struct
{
	char name[10]; //TODO: change to char*
	driver_t type;
	void* driver;
    // int (*open) (void);
	// void (*close) (void);
} device_driver_t;

typedef struct
{
	int (*read) (int *buffer, unsigned int count);
	int (*ioctl) (unsigned int num, unsigned long param);
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
	int (*read) (unsigned char *buffer, unsigned int sector);
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
device_driver_t* driver_find_device_id(int id);

#endif
