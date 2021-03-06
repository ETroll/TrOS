#include <tros/klib/devicebuffer.h>
#include <tros/memory.h>

devicebuffer_t* devicebuffer_create()
{
    devicebuffer_t* buffer = (devicebuffer_t*)kmalloc(sizeof(devicebuffer_t));
    buffer->ringbuffer = (ringbuffer_t*)kmalloc(sizeof(ringbuffer_t));
    buffer->listener = 0;
    rb_init(buffer->ringbuffer);

    return buffer;
}

void devicebuffer_free(devicebuffer_t* db)
{
    kfree(db->ringbuffer);
    kfree(db);
}

int devicebuffer_read(devicebuffer_t* db, int* buffer, uint32_t count)
{
    int read = 0;
    if(db->listener == 0)
    {
        db->listener = scheduler_getCurrentThread();
        do
        {
            if(rb_len(db->ringbuffer) > 0)
            {
                rb_pop(db->ringbuffer, &buffer[read++]);
            }
            else
            {
                thread_setState(db->listener, THREAD_WAITIO);
                scheduler_reschedule();
            }
        } while(read < count);
        db->listener = 0;
    }
    return read;
}

void devicebuffer_write(devicebuffer_t* db, int data)
{
    if(db != 0)
    {
        rb_push(db->ringbuffer, data);
        if(db->listener != 0)
        {
            thread_setState(db->listener, THREAD_IOREADY);
        }
    }
}
