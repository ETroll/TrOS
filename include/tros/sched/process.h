#ifndef INCLUDE_TROS_PROCESS_H
#define INCLUDE_TROS_PROCESS_H

#include <stdint.h>
#include <tros/mailbox.h>
#include <tros/klib/list.h>
#include <tros/mem/vmm2.h>

#define PROCESS_MEM_START 0x400000

typedef struct process
{
    list_t* threads;
    list_t* children;
    page_directory_t* pagedir;
    struct process *parent;
    mailbox_t* mailbox;
    uint32_t pid;
    uint32_t heapendAddr;
    char** argv;
    int argc;
} process_t;

process_t* process_create(process_t* parent);
void process_dispose(process_t* proc);


#endif
