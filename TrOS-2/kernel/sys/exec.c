#include <tros/process.h>
#include <tros/exec.h>
#include <tros/tros.h>
#include <tros/kheap.h>
#include <tros/vmm.h>
#include <string.h>
#include <tros/fs/vfs.h>
#include <tros/sys/elf32.h>

//NOTE: Thos method jumps right in to the new code in Userland
int exec_elf32(char* path, int argc, char** argv)
{
    fs_node_t* file = kopen(path);
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
                pdirectory_t* pagedir = vmm_clone_directory(vmm_get_directory());
                vmm_switch_pdirectory(pagedir);

                printk("We have a valid executable with entry at %x\n", elf_header.e_entry);
                printk("\nType Offset      VirtAddr    PhysAddr    FileSiz     MemSiz     Align\n");

                for(int i = 0; i<elf_header.e_phnum; i++)
                {
                    Elf32_ProgramHeader_t pgr_h;
                    vfs_read(file,
                        elf_header.e_phoff + (i * elf_header.e_phentsize),
                        elf_header.e_phentsize,
                        (unsigned char *)&pgr_h);

                    printk("%d    %x  %x  %x  %x  %x %x\n",
                        pgr_h.p_type,
                        pgr_h.p_offset,
                        pgr_h.p_vaddr,
                        pgr_h.p_paddr,
                        pgr_h.p_filesz,
                        pgr_h.p_memsz,
                        pgr_h.p_align);

                    if(pgr_h.p_type == ELF32_PT_LOAD
                    && pgr_h.p_align == VMM_BLOCK_ALIGN)
                    {
                        vmm_create_and_map(pgr_h.p_vaddr, pgr_h.p_memsz, VMM_FLAG_USER);
                        if(pgr_h.p_filesz != pgr_h.p_memsz)
                        {
                            memset((void *)(pgr_h.p_vaddr + pgr_h.p_filesz),
                                '\0',
                                (pgr_h.p_memsz-pgr_h.p_filesz));
                        }
                        if(vfs_read(file, pgr_h.p_offset, pgr_h.p_filesz, (unsigned char *)pgr_h.p_vaddr) != pgr_h.p_filesz)
                        {
                            printk("Error reading segment\n");
                        }
                    }
                    else
                    {
                        printk("Unknown program segment type or block size\n", path);
                        //Maybe stop from executing if we occur this?
                    }
                }
                //TODO: Execute!
                //All just temp for now
                uint32_t ustackAddr = 0x400000;
                vmm_map_create_page(ustackAddr, VMM_FLAG_USER);
                ustackAddr += (VMM_BLOCK_SIZE - sizeof(unsigned int));

                process_exec_user(elf_header.e_entry,
                    ustackAddr,
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
        //NOTE: THis will leak... hm..
        vfs_close(file);
    }
    else
    {
        printk("Could not find file: %s\n", path);
    }

    return 0;
}
