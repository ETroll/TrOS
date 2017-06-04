#include <tros/elf.h>
#include <tros/memory.h>
#include <tros/mem/vmm2.h>
#include <tros/klib/kstring.h>
#include <tros/fs/vfs.h>
#include <tros/tros.h>

uint32_t elf32_load(char* path, uint32_t* startAddr)
{
    fs_node_t* file = kopen(path);
    printk("File: %x", file);
    uint32_t highetsAddr = 0;
    *startAddr = 0;

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
                printk("\nWe have a valid executable with entry at %x\n", elf_header.e_entry);
                printk("\nType Offset      VirtAddr    PhysAddr    FileSize    MemSize    Align\n");

                *startAddr = elf_header.e_entry;
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
                        if(pgr_h.p_memsz % VMM2_BLOCK_SIZE > 0)
                        {
                            blocks++;
                        }
                        printk("- %d blocks\n", blocks);

                        //NOTE: Application is writable. Separate .data and .bss
                        //      from .text
                        vmm2_map(pgr_h.p_vaddr, blocks, VMM2_PAGE_USER | VMM2_PAGE_WRITABLE);

                        if((pgr_h.p_vaddr + (VMM2_BLOCK_SIZE * blocks)) > highetsAddr)
                        {
                            highetsAddr = pgr_h.p_vaddr + (VMM2_BLOCK_SIZE * blocks);
                        }

                        if(pgr_h.p_filesz != pgr_h.p_memsz)
                        {
                            memset((void *)(pgr_h.p_vaddr + pgr_h.p_filesz),
                                '\0',
                                (pgr_h.p_memsz-pgr_h.p_filesz));
                        }
                        int tmpread = 0;
                        if((tmpread = vfs_read(file, pgr_h.p_offset, pgr_h.p_filesz, (unsigned char *)pgr_h.p_vaddr)) != pgr_h.p_filesz)
                        {
                            printk("elf32_load: Error reading segment. Expected %d got %d\n", pgr_h.p_filesz, tmpread);
                            highetsAddr = 0;
                            *startAddr = 0;
                            break;
                        }
                    }
                    else
                    {
                        printk("elf32_load: Unknown program segment type or block size\n");
                        highetsAddr = 0;
                        *startAddr = 0;
                        break;
                    }
                }
            }
            else
            {
                printk("elf32_load: %s is not an ELF32 executable!\n", path);
            }
        }
        else
        {
            printk("elf32_load: Failure reading file %s\n", path);
        }
        vfs_close(file);
    }
    else
    {
        printk("elf32_load: Could not find file %s\n", path);
    }
    return highetsAddr;
}
