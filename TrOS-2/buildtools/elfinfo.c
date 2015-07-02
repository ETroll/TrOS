
#include <stdio.h>

#define EI_NIDENT 16

typedef unsigned short Elf32_Half;
typedef unsigned int Elf32_Word;
typedef unsigned int Elf32_Addr;
typedef unsigned int Elf32_Off;

const char* Elf32_Shdr_Types[] = {
    "SHT_NULL",
    "SHT_PROGBITS",
    "SHT_SYMTAB",
    "SHT_STRTAB",
    "SHT_RELA",
    "SHT_HASH",
    "SHT_DYNAMIC",
    "SHT_NOTE",
    "SHT_NOBITS",
    "SHT_REL",
    "SHT_SHLIB",
    "SHT_DYNSYM"
};

const char* Elf32_Ehdr_Types[] = {
    "ET_NONE",
    "ET_REL",
    "ET_EXEC",
    "ET_DYN",
    "ET_CORE"
};

const char* Elf32_Ehdr_Machines[] = {
    "EM_NONE",
    "EM_M32",
    "EM_SPARC",
    "EM_386",
    "EM_68K",
    "EM_88K",
    "EM_860",
    "EM_MIPS"
};

const char* Elf32_Phdr_Types[] = {
    "PT_NULL",
    "PT_LOAD",
    "PT_DYNAMIC",
    "PT_INTERP",
    "PT_NOTE",
    "PT_SHLIB",
    "PT_PHDR"
};

typedef struct {
    unsigned char e_ident[EI_NIDENT];
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
} Elf32_Ehdr;

typedef struct {
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

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
} Elf32_Shdr;

void PrintElfHeader(Elf32_Ehdr* hdr);
void PrintProgramHeader(Elf32_Phdr* hdr);
void PrintfSectionHeader(Elf32_Shdr* hdr);

int main(int argc, char const *argv[]) {
    printf("ELF File information:\n\n");

    if(argc != 2)
    {
        printf("Invalid arguments. Usage elfinfo <path>\n");
    }
    else
    {
        FILE *pELF = fopen(argv[1], "rb");
		if (!pELF)
		{
			printf("Unable to open file: %s\n", argv[1]);
		}
        else
        {
            Elf32_Ehdr elf_header;

            long file_size = 0;
            fseek(pELF , 0 , SEEK_END);
            file_size = ftell(pELF);
            rewind(pELF);

            fread(&elf_header,sizeof(Elf32_Ehdr), 1 , pELF);
            printf("File size: %ld bytes\n--------\n\n", file_size);

            PrintElfHeader(&elf_header);

            fseek(pELF, elf_header.e_phoff, SEEK_SET);
            for(int i = 0; i<elf_header.e_phnum; i++)
            {
                Elf32_Phdr program_header;
                fread(&program_header,sizeof(Elf32_Phdr), 1 , pELF);
                PrintProgramHeader(&program_header);
            }

            fseek(pELF, elf_header.e_shoff, SEEK_SET);
            for(int i = 0; i<elf_header.e_shnum; i++)
            {
                Elf32_Shdr section_header;
                fread(&section_header,sizeof(Elf32_Shdr), 1 , pELF);
                PrintfSectionHeader(&section_header);
            }

            fclose(pELF);
        }

    }

    return 0;
}

void PrintElfHeader(Elf32_Ehdr* hdr)
{
    printf("ELF Header: \n--------\n");
    printf("e_ident:\t");
    for(int i = 0; i< EI_NIDENT; i++)
    {
        printf(" %02x", hdr->e_ident[i]);
    }
    printf("\n");

    printf("e_type:\t\t 0x%04x", hdr->e_type);
    printf("\t\t%s \tElf type\n", Elf32_Ehdr_Types[hdr->e_type]);

    printf("e_machine:\t 0x%04x", hdr->e_machine);
    printf("\t\t%s \t\tMachine type\n", Elf32_Ehdr_Machines[hdr->e_machine]);

    printf("e_version:\t 0x%08x\n", hdr->e_version);
    printf("e_entry:\t 0x%08x", hdr->e_entry);
    printf("\t\t\tEntry address (VMem)\n");

    printf("e_phoff:\t 0x%08x", hdr->e_phoff);
    printf("\t%i bytes \tProgram header table offset\n", hdr->e_phoff);

    printf("e_shoff:\t 0x%08x", hdr->e_shoff);
    printf("\t%i bytes \tSection header table offset\n", hdr->e_shoff);

    printf("e_flags:\t 0x%08x\n", hdr->e_flags);

    printf("e_ehsize:\t 0x%04x", hdr->e_ehsize);
    printf("\t\t%i bytes \tHeader size\n", hdr->e_ehsize);

    printf("e_phentsize:\t 0x%04x", hdr->e_phentsize);
    printf("\t\t%i bytes \tProgram header size\n", hdr->e_phentsize);

    printf("e_phnum:\t 0x%04x", hdr->e_phnum);
    printf("\t\t%i \t\tNumber of program headers\n", hdr->e_phnum);

    printf("e_shentsize:\t 0x%04x", hdr->e_shentsize);
    printf("\t\t%i bytes \tSection header size\n", hdr->e_shentsize);

    printf("e_shnum:\t 0x%04x", hdr->e_shnum);
    printf("\t\t%i \t\tNumber of section headers\n", hdr->e_shnum);

    printf("e_shstrndx:\t 0x%04x", hdr->e_shstrndx);
    printf("\t\t%i \t\tEntry index\n", hdr->e_shstrndx);
    printf("\n");
}

void PrintProgramHeader(Elf32_Phdr* hdr)
{
    printf("Program Header: \n--------\n");

    printf("p_type:\t\t 0x%08x", hdr->p_type);
    printf("\t%s \tSection type\n", Elf32_Phdr_Types[hdr->p_type]);

    printf("p_offset:\t 0x%08x", hdr->p_offset);
    printf("\t%i bytes \tELF file segments offset\n", hdr->p_offset);

    printf("p_vaddr:\t 0x%08x", hdr->p_vaddr);
    printf("\t\t\tVirtual address (first segment)\n");

    printf("p_paddr:\t 0x%08x", hdr->p_paddr);
    printf("\t\t\tPhysical address (first segment, if applicable)\n");

    printf("p_filesz:\t 0x%08x", hdr->p_filesz);
    printf("\t%i bytes \tFile size\n", hdr->p_filesz);

    printf("p_memsz:\t 0x%08x", hdr->p_memsz);
    printf("\t%i bytes \tMemory size\n", hdr->p_memsz);

    printf("p_flags:\t 0x%08x\n", hdr->p_flags);
    printf("p_align:\t 0x%08x", hdr->p_align);
    printf("\t%i bytes \tAlignment\n", hdr->p_align);

    printf("\n");
}

void PrintfSectionHeader(Elf32_Shdr* hdr)
{
    printf("Section Header: \n--------\n");

    printf("sh_name:\t 0x%08x", hdr->sh_name);
    printf("\t%i \t\tSection header string table index\n", hdr->sh_name);

    printf("sh_type:\t 0x%08x", hdr->sh_type);
    printf("\t%s \tSection type\n", Elf32_Shdr_Types[hdr->sh_type]);

    printf("sh_flags:\t 0x%08x\n", hdr->sh_flags);
    printf("sh_addr:\t 0x%08x", hdr->sh_addr);
    printf("\t\t\tVirtual address\n");

    printf("sh_offset:\t 0x%08x", hdr->sh_offset);
    printf("\t%i bytes \tELF file offset\n", hdr->sh_offset);

    printf("sh_size:\t 0x%08x", hdr->sh_size);
    printf("\t%i bytes \tSection size\n", hdr->sh_size);

    printf("sh_link:\t 0x%08x\n", hdr->sh_link);
    printf("sh_info:\t 0x%08x\n", hdr->sh_info);
    printf("sh_addralign:\t 0x%08x\n", hdr->sh_addralign);
    printf("sh_entsize:\t 0x%08x\n", hdr->sh_entsize);

    printf("\n");
}
