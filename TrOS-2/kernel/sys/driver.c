#include <tros/driver.h>
#include <tros/ds/list.h>
#include <tros/kheap.h>
#include <string.h>

list_t* _drivers = 0;

int driver_register(device_driver_t* driver)
{
    if(_drivers == 0)
    {
        _drivers = (list_t*)kmalloc(sizeof(list_t));
        _drivers->head = 0;
        _drivers->size = 0;
    }

    list_node_t* node = _drivers->head;
    while(node != 0)
    {
        device_driver_t* device = (device_driver_t*)node->data;
        if(strcmp(device->name, driver->name) == 0)
        {
            return -1;
        }
        node = node->next;
    }

    device_driver_t* data = (device_driver_t*)kmalloc(sizeof(device_driver_t));
    data->driver = (void*)driver->driver;
    data->type = driver->type;
    strcpy(data->name, driver->name);

    list_add(_drivers, data);
    return _drivers->size;
}

device_driver_t* driver_find_device(char* name)
{
    list_node_t* node = _drivers->head;
    device_driver_t* driver = 0;

    while(node != 0)
    {
        device_driver_t* device = (device_driver_t*)node->data;
        if(strcmp(device->name, name) == 0)
        {
            driver = device;
            break;
        }
        node = node->next;
    }
    return driver;
}

device_driver_t* driver_find_device_id(int id)
{
    list_node_t* node = list_get_at_index(_drivers, id);
    if(node != 0)
    {
        return (device_driver_t*)node->data;
    }
    else
    {
        return 0;
    }
}
