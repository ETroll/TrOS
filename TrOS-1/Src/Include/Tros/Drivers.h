#ifndef SRC_INCLUDE_KERNEL_DRIVER_H_
#define SRC_INCLUDE_KERNEL_DRIVER_H_

#define MAX_DRIVERS 10

typedef enum
{
	DRV_NONE,
	DRV_SERIAL,
	DRV_FRAMEBUFFER
} driver_t;

struct device_driver
{
	char name[100];
	int device_id;
	driver_t type;
	void* driver;
};

struct serial_driver
{
	int (*read) (char *buffer, unsigned int count);
	int (*write) (const char *buffer, unsigned int count);
	int (*writechar) (char c);
	int (*peek) (void);	//Dont know if I want a Peek, but this solves a temporary problem for now
	int (*open) (void);
	int (*close) (void);
};

void init_device_driver_struct(void);

int drivers_register_serial(struct serial_driver* driver, char* name);
struct device_driver* drivers_find_device(char* name);
struct device_driver* drivers_find_device_id(int id);




//TEMP init methods signatures for current drivers.
int uart_init_driver(void);
int spi_init_driver(void);

#endif /* SRC_INCLUDE_KERNEL_DRIVER_H_ */
