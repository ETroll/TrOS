#ifndef INCLUDE_TROS_ELF_H
#define INCLUDE_TROS_ELF_H

#include <stdint.h>

typedef unsigned char Elf32_Flag;
typedef unsigned short Elf32_Half;
typedef unsigned int Elf32_Word;
typedef unsigned int Elf32_Addr;
typedef unsigned int Elf32_Off;

typedef struct {
    Elf32_Word e_magic;
    Elf32_Flag e_bit;
    Elf32_Flag e_endian;
    Elf32_Flag e_tinyversion;   //NOTE: Look up difference from other version at
                                //      position 20-23 in header.
    Elf32_Flag e_abi;
    Elf32_Flag e_unused_padding[8];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off  e_phoff;
    Elf32_Off  e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Header_t;

typedef struct {
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_ProgramHeader_t;

typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off  sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_SectionHeader_t;

enum Elf32_Filetype {
    ELF32_FILE_RELOCATABLE = 1,
    ELF32_FILE_EXECUTABLE,
    ELF32_FILE_SHARED,
    ELF32_FILE_CORE
};

enum Elf32_ProgramEntryType {
    ELF32_PT_NULL = 0,
    ELF32_PT_LOAD,
    ELF32_PT_DYNAMIC,
    ELF32_PT_INTERP,
    ELF32_PT_NOTE,
    ELF32_PT_SHLIB,
    ELF32_PT_PHDR,
    ELF32_PT_LOOS,
    ELF32_PT_HIOS,
    ELF32_PT_LOPROC,
    ELF32_PT_HIPROC
};

uint32_t elf32_load(char* path, uint32_t* startAddr);

#endif
