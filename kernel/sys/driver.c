#include <tros/driver.h>
#include <tros/klib/list.h>
#include <tros/memory.h>
#include <tros/klib/kstring.h>
#include <tros/tros.h>

static list_t* _drivers = 0;

int driver_register(device_driver_t* device)
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
        device_driver_t* tmpdevice = (device_driver_t*)node->data;
        if(strcmp(tmpdevice->name, device->name) == 0)
        {
            return -1;
        }
        node = node->next;
    }

    device_driver_t* data = (device_driver_t*)kmalloc(sizeof(device_driver_t));
    data->driver = (void*)device->driver;
    data->type = device->type;
    data->id = _drivers->size;  //TODO: Unsafe, when removal of drivers gets
                                //      implemented, then this is no longer valid
    strcpy(data->name, device->name);

    list_add(_drivers, data);
    return _drivers->size;
}

device_driver_t* driver_find_device(const char* name)
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

device_driver_t* driver_find_device_id(unsigned int id)
{
    list_node_t* node = list_get_at_index(_drivers, id);
    if(node != 0)
    {
        //device_driver_t* device = (device_driver_t*)node->data;
        //printk("Found: %s(%i) for ID: %i\n", device->name, device->id, id);
        return (device_driver_t*)node->data;
    }
    else
    {
        return 0;
    }
}
