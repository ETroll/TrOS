#include <Tros/Drivers.h>
#include <Tros/Utils.h>

struct device_driver drivers[MAX_DRIVERS];
int next_driver_id = 0;

void init_device_driver_struct()
{
	next_driver_id = 0;
	for (int i = 0; i < MAX_DRIVERS; i++)
	{
		 drivers[i].driver = 0;
		 drivers[i].device_id = 0;
		 drivers[i].name[0] = '\0';
		 drivers[i].type = DRV_NONE;
	}
}

int drivers_register_serial(struct serial_driver* driver, char* name)
{
	for(int i = 0; i<next_driver_id; i++)
	{
		if(_k_strcmp(drivers[i].name, name) == 0)
		{
			return -1;
		}
	}

	drivers[next_driver_id].driver = (void*) driver;
	drivers[next_driver_id].type = DRV_SERIAL;
	drivers[next_driver_id].device_id = next_driver_id+1;
	_k_strcpy(drivers[next_driver_id].name, name);

	next_driver_id++;
	return drivers[next_driver_id].device_id;
}

struct device_driver* drivers_find_device(char* name)
{
	struct device_driver* driver = NULL;

	for (int i = 0; i < MAX_DRIVERS; i++)
	{
		if(_k_strcmp(drivers[i].name, name) == 0)
		{
			driver = &drivers[i];
			break;
		}
	}

	return driver;
}

struct device_driver* drivers_find_device_id(int id)
{
	struct device_driver* driver = NULL;
	if(id <= MAX_DRIVERS)
	{
		for (int i = 0; i < MAX_DRIVERS; i++)
		{
			if(drivers[i].device_id == id)
			{
				driver = &drivers[i];
				break;
			}
		}
	}
	return driver;
}



