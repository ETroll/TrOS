#include <tros/driver.h>
#include <string.h>

#define MAX_DRIVERS 10

static device_driver_t drivers[MAX_DRIVERS];
static int next_driver_id = 0;

void driver_initialize()
{
    next_driver_id = 0;
    for (int i = 0; i < MAX_DRIVERS; i++)
    {
        drivers[i].driver = 0;
        drivers[i].name[0] = '\0';
        drivers[i].type = DRV_NONE;
    }
}
int driver_register(device_driver_t* driver)
{
    for(int i = 0; i<next_driver_id; i++)
    {
        if(strcmp(drivers[i].name, driver->name) == 0)
        {
            return -1;
        }
    }

    drivers[next_driver_id].driver = (void*)driver->driver;
    drivers[next_driver_id].type = driver->type;
    strcpy(drivers[next_driver_id].name, driver->name);

    return next_driver_id++;
}

device_driver_t* driver_find_device(char* name)
{
    device_driver_t* driver = 0;
    for (int i = 0; i < MAX_DRIVERS; i++)
    {
        if(strcmp(drivers[i].name, name) == 0)
        {
            driver = &drivers[i];
            break;
        }
    }
    return driver;
}

device_driver_t* driver_find_device_id(int id)
{
	return &drivers[id];
}
