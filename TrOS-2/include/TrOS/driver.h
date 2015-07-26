// driver.h
// Primitive driver system

#ifndef INCLUDE_TROS_DRIVER_H
#define INCLUDE_TROS_DRIVER_H

typedef enum
{
    DRV_NONE,
	DRV_SERIAL,
    DRV_BLOCK,
    DRV_HID
} driver_t;

typedef struct
{
	char name[10]; //TODO: change to char*
	driver_t type;
	void* driver;
} device_driver_t;

typedef struct
{
	int (*read) (char *buffer, unsigned int count);
	int (*ioctl) (unsigned int num, unsigned long param);
    //int (*open) (void);
	//int (*close) (void);
} driver_hid_t;

//Set up the driver system
void driver_initialize();
int driver_register(device_driver_t* driver);


#endif
