
#include <tros/exec.h>
#include <tros/tros.h>
#include <tros/kheap.h>
#include <tros/vmm.h>

//NOTE: Thos method jumps right in to the new code in Userland
int exec_elf32(char* path, int argc, char** argv)
{

    pdirectory_t* pagedir = vmm_clone_directory(vmm_get_directory());
    vmm_switch_pdirectory(pagedir);


    // process_exec_user(uint32_t startAddr, uint32_t ustack, uint32_t kstack, pdirectory_t* pdir)



    return 0;
}
