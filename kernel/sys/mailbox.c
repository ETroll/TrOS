#include <tros/mailbox.h>
#include <tros/atomics.h>
#include <tros/memory.h>
#include <tros/klib/kstring.h>
#include <tros/sched/scheduler.h>


static mailbox_message_t* mailbox_removetop(mailbox_t* mb);

mailbox_t* mailbox_create()
{
    mailbox_t* mb = (mailbox_t*)kmalloc(sizeof(mailbox_t));
    if(mb)
    {
        mb->totalbytes = 0;
        mb->size = 0;
        mb->start = 0;
        mb->end = 0;
        mb->listener = 0;
        spinlock_inistialize(mb->memlock);
    }
    return mb;
}

void mailbox_dispose(mailbox_t* mb)
{
    mailbox_message_t* tmp = mb->start;
    while(tmp != 0)
    {
        mailbox_message_t* curr = tmp;
        tmp = curr->next;
        mailbox_message_dispose(curr);
    }
    kfree(mb);
}

void mailbox_push(mailbox_t* mb, mailbox_message_t* message)
{
    if(mb && message)
    {
        if(message->size < MAILBOX_MAX_SIZE)
        {
            // spinlock_lock(mb->memlock);
            while((mb->totalbytes + message->size) > MAILBOX_MAX_SIZE)
            {
                mailbox_message_t* message = mailbox_removetop(mb);
                if(message)
                {
                    mailbox_message_dispose(message);
                }
            }
            if(mb->end == 0)
            {
                mb->end = message;
                mb->start = message;
                mb->totalbytes = message->size;
                mb->size = 1;
            }
            else
            {
                mb->end->next = message;
                mb->end = message;
                mb->totalbytes += message->size;
                mb->size++;
            }
            // spinlock_unlock(mb->memlock);
            if(mb->listener != 0)
            {
                // printk("TID %d got IO\n", ((thread_t*)mb->listener)->tid);
                thread_setState((thread_t*)mb->listener, THREAD_IOREADY);
            }
        }
    }
    //NOTE: Maybe return something?
}

mailbox_message_t* mailbox_pop(mailbox_t* mb, uint32_t flags)
{
    mailbox_message_t* message = 0;
    if(mb)
    {
        if(mb->listener == 0)
        {
            uint32_t size = 0;
            // spinlock_lock(mb->memlock);
            size = mb->size;
            // spinlock_unlock(mb->memlock);

            if(size == 0 && !(flags & 0x02))
            {
                mb->listener = scheduler_getCurrentThread();
                thread_setState((thread_t*)mb->listener, THREAD_WAITIO);
                // printk("TID %d going to sleep\n", ((thread_t*)mb->listener)->tid);
                scheduler_reschedule();
                // printk("TID %d woke up\n", ((thread_t*)mb->listener)->tid);
                mb->listener = 0;
            }

            // spinlock_lock(mb->memlock);
            message = mailbox_removetop(mb);
            // spinlock_unlock(mb->memlock);
        }
    }
    return message;
}

mailbox_message_t* mailbox_removetop(mailbox_t* mb)
{
    mailbox_message_t* message = mb->start;
    if(message)
    {
        if(message == mb->end)
        {
            mb->totalbytes = 0;
            mb->size = 0;
            mb->start = 0;
            mb->end = 0;
        }
        else
        {
            mb->start = message->next;
            mb->totalbytes -= message->size;
            mb->size--;
        }
    }
    return message;
}

mailbox_message_t* mailbox_message_create(uint32_t sender, const void* payload,
     uint32_t size, uint32_t flags)
{
    mailbox_message_t* message = (mailbox_message_t*)kmalloc(sizeof(mailbox_message_t));
    if(message)
    {
        message->size = size;
        message->senderpid = sender;
        message->payload = kmalloc(size);
        memcpy(message->payload, (void*)payload, size);
        message->reciept = flags & 0x01;
        message->next = 0;
    }
    return message;
}

void mailbox_message_dispose(mailbox_message_t* message)
{
    if(message)
    {
        if(message->payload)
        {
            kfree(message->payload);
        }
        kfree(message);
    }
}
