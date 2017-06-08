#ifndef INCLUDE_TROS_MAILBOX_H
#define INCLUDE_TROS_MAILBOX_H

#include <tros/atomics.h>
#include <stdint.h>

#define MAILBOX_MAX_SIZE 1048576

typedef struct mailbox_message
{
    uint32_t size;
    uint32_t senderpid;
    void* payload;
    int8_t reciept:1;
    struct mailbox_message* next;

} mailbox_message_t;

typedef struct
{
    uint32_t totalbytes;
    uint32_t size;
    spinlock_t memlock;
    void* listener;
    mailbox_message_t* start;
    mailbox_message_t* end;
} mailbox_t;


mailbox_t* mailbox_create();
void mailbox_dispose(mailbox_t* mb);

void mailbox_push(mailbox_t* mb, mailbox_message_t* message);
mailbox_message_t* mailbox_pop(mailbox_t* mb, uint32_t flags);

mailbox_message_t* mailbox_message_create(uint32_t sender, const void* payload,
    uint32_t size, uint32_t flags);
void mailbox_message_dispose(mailbox_message_t* message);

#endif
