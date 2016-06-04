#include <tros/fs.h>
#include <tros/klib/list.h>
#include <tros/kheap.h>
#include <tros/klib/kstring.h>

list_t* _fs_filesystems = 0;

int fs_register(filesystem_t* fs)
{
    if(_fs_filesystems == 0)
    {
        _fs_filesystems = (list_t*)kmalloc(sizeof(list_t));
        _fs_filesystems->head = 0;
        _fs_filesystems->size = 0;
    }

    filesystem_t* data = (filesystem_t*)kmalloc(sizeof(filesystem_t));
    data->name = (char*)kmalloc(strlen(fs->name)+1);

    strcpy(data->name, fs->name);
    data->fsops = fs->fsops;
    data->mount = fs->mount;
    list_add(_fs_filesystems, data);

    return _fs_filesystems->size;
}

filesystem_t* fs_lookup(char* name)
{
    list_node_t* node = _fs_filesystems->head;
    filesystem_t* fs = 0;

    while(node != 0)
    {
        filesystem_t* tmp = (filesystem_t*)node->data;
        if(strcmp(tmp->name, name) == 0)
        {
            fs = tmp;
            break;
        }
        node = node->next;
    }
    return fs;
}
