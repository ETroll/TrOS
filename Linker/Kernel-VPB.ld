MEMORY
{
    ram : ORIGIN = 0x10000, LENGTH = 0x10000
}

SECTIONS
{
    .text : { *(.text*) } > ram
    .bss : { *(.bss*) } > ram
}
