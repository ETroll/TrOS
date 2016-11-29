
#include <tros/exec.h>
#include <tros/tros.h>
#include <tros/kheap.h>
#include <tros/vmm.h>
#include <tros/fs/vfs.h>

//NOTE: Thos method jumps right in to the new code in Userland
int exec_elf32(char* path, int argc, char** argv)
{

    pdirectory_t* pagedir = vmm_clone_directory(vmm_get_directory());
    vmm_switch_pdirectory(pagedir);

    //

    fs_node_t* file = kopen(path);
    if(file != 0)
    {
        printk("Found BINARY at %d size: %d - Executing!\n", file->inode, file->size);
        //TODO: Maybe get this into VMM instead, and change it here and in kmalloc

        uint32_t blocks = file->size / VMM_BLOCK_SIZE;
        if(file->size % VMM_BLOCK_SIZE > 0) blocks ++;

        printk("%d blocks\n", blocks);
        uint32_t startAddr = 0x200000;
        uint32_t endAddr = startAddr + (VMM_BLOCK_SIZE * blocks);
        for(uint32_t i = startAddr; i < endAddr; i += VMM_BLOCK_SIZE)
        {
            vmm_map_create_page(i, VMM_FLAG_USER);
        }

        printk("Created pages! Now to read the file.\n");
        unsigned int read_bytes = vfs_read(file, 0, file->size, (unsigned char*)startAddr);
        if(read_bytes == file->size)
        {
            //All just temp for now
            uint32_t ustackAddr = 0x400000;

            vmm_map_create_page(ustackAddr, VMM_FLAG_USER);
            process_exec_user(0x200000,
                ustackAddr,
                (unsigned int)kmalloc(16384) + (16384 - sizeof(unsigned int)),
                pagedir);
        }
        else
        {
            printk("Failure reading file %s\n", path);
        }
        //NOTE: THis will leak... hm..
        vfs_close(file);
    }
    else
    {
        printk("Could not find file: %s\n", path);
    }

    return 0;
}
