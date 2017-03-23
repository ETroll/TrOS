#include <tros/process.h>
#include <tros/exec.h>
#include <tros/tros.h>
#include <tros/memory.h>
#include <tros/mem/vmm2.h>
#include <tros/klib/kstring.h>
#include <tros/fs/vfs.h>
#include <tros/sys/elf32.h>


int exec_elf32(char* path, int argc, char** argv)
{
    // printk("Trying to find file %s\n", path);
    fs_node_t* file = kopen(path);
    uint32_t newpid = 0;
    // printk("File: %x\n", file);
    if(file != 0)
    {
        Elf32_Header_t elf_header;
        unsigned int read_bytes = vfs_read(file, 0, sizeof(Elf32_Header_t),
            (unsigned char *)&elf_header);

        if(read_bytes == sizeof(Elf32_Header_t))
        {
            if(elf_header.e_type == ELF32_FILE_EXECUTABLE
            && elf_header.e_magic == 0x464C457F //0x7F + ELF in little endian
            && sizeof(Elf32_ProgramHeader_t) == elf_header.e_phentsize) //will fail if 64bit
            {
                //TODO: Save old pagedir so we can fall back
                // page_directory_t* olddir = vmm2_get_directory()
                page_directory_t* pagedir = vmm2_create_directory();
                // vmm2_switch_pagedir(pagedir);
                uint32_t highetsAddr = 0;

                printk("\nWe have a valid executable with entry at %x\n", elf_header.e_entry);
                printk("\nType Offset      VirtAddr    PhysAddr    FileSiz     MemSiz     Align\n");

                for(int i = 0; i<elf_header.e_phnum; i++)
                {
                    Elf32_ProgramHeader_t pgr_h;
                    vfs_read(file,
                        elf_header.e_phoff + (i * elf_header.e_phentsize),
                        elf_header.e_phentsize,
                        (unsigned char *)&pgr_h);

                    printk("%d    %x  %x  %x  %x  %x %x ",
                        pgr_h.p_type,
                        pgr_h.p_offset,
                        pgr_h.p_vaddr,
                        pgr_h.p_paddr,
                        pgr_h.p_filesz,
                        pgr_h.p_memsz,
                        pgr_h.p_align);

                    if(pgr_h.p_type == ELF32_PT_LOAD
                    && pgr_h.p_align == VMM2_BLOCK_SIZE)
                    {
                        unsigned int blocks = pgr_h.p_memsz / VMM2_BLOCK_SIZE;
                        if(pgr_h.p_memsz % VMM2_BLOCK_SIZE > 0) blocks++;
                        printk("- %d blocks\n", blocks);
                        //NOTE: Application is writable. Separate .data and .bss
                        //      from .text
                        vmm2_map(pgr_h.p_vaddr, blocks, VMM2_PAGE_USER | VMM2_PAGE_WRITABLE, pagedir);

                        if((pgr_h.p_vaddr + (VMM2_BLOCK_SIZE * blocks)) > highetsAddr)
                        {
                            highetsAddr = pgr_h.p_vaddr + (VMM2_BLOCK_SIZE * blocks);
                        }

                        // if(pgr_h.p_filesz != pgr_h.p_memsz)
                        // {
                        //     memset((void *)(pgr_h.p_vaddr + pgr_h.p_filesz),
                        //         '\0',
                        //         (pgr_h.p_memsz-pgr_h.p_filesz));
                        //     // printk("Zeroed: %x to %x\n", (pgr_h.p_vaddr + pgr_h.p_filesz),
                        //     //     (pgr_h.p_vaddr + pgr_h.p_filesz) + (pgr_h.p_memsz-pgr_h.p_filesz));
                        // }
                        int tmpread = 0;
                        unsigned char* buffer = (unsigned char*)kmalloc(pgr_h.p_filesz);
                        // if((tmpread = vfs_read(file, pgr_h.p_offset, pgr_h.p_filesz, (unsigned char *)pgr_h.p_vaddr)) != pgr_h.p_filesz)
                        // {
                        //     printk("Error reading segment. Expected %d got %d\n", pgr_h.p_filesz, tmpread);
                        // }
                        if((tmpread = vfs_read(file, pgr_h.p_offset, pgr_h.p_filesz, buffer)) == pgr_h.p_filesz)
                        {
                            if(pgr_h.p_filesz != pgr_h.p_memsz)
                            {
                                vmm2_memset((void *)(pgr_h.p_vaddr + pgr_h.p_filesz),
                                    '\0',
                                    (pgr_h.p_memsz-pgr_h.p_filesz),
                                    pagedir);
                            }
                            vmm2_memcpy((void*)pgr_h.p_vaddr, buffer, pgr_h.p_filesz, pagedir);
                        }
                        else
                        {
                            printk("Error reading segment. Expected %d got %d\n", pgr_h.p_filesz, tmpread);
                        }
                        kfree(buffer);
                    }
                    else
                    {
                        printk("Unknown program segment type or block size\n", path);
                        //Maybe stop from executing if we occur this?
                    }
                }
                //All just temp for now
                uint32_t ustackAddr = 0xBFFFC000; //16k below kernel
                vmm2_map_todir(ustackAddr, 1, VMM2_PAGE_USER | VMM2_PAGE_WRITABLE, pagedir); //1 block - 4K stack
                // ustackAddr += (VMM2_BLOCK_SIZE - (sizeof(unsigned int)*2));
                ustackAddr += (VMM2_BLOCK_SIZE - sizeof(unsigned int));
                printk("\n");

                printk("Executing process with:\n");
                printk("  Entry: %x\n", elf_header.e_entry);
                printk("  Stack: %x\n", ustackAddr);
                printk("  Heapstart: %x\n\n", highetsAddr);

                newpid = process_exec_user(elf_header.e_entry,
                    ustackAddr,
                    highetsAddr,
                    (unsigned int)kmalloc(16384) + (16384 - sizeof(unsigned int)),
                    pagedir);
            }
            else
            {
                printk("File %s is not an executable!\n", path);
            }
        }
        else
        {
            printk("Failure reading file %s\n", path);
        }
        vfs_close(file);
    }
    else
    {
        printk("Could not find file: %s\n", path);
    }
    printk("Completely executed file %s\n", path);
    return newpid;
}
