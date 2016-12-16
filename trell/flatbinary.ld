ENTRY(main)
SECTIONS
{
    . = 0x200000;
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.text)
    }

    .data BLOCK(4K): ALIGN(4K)
    {
        *(.data)
        *(.rodata)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }

    /DISCARD/ :
    {
        *(.eh_frame)
    }

    end = .; _end = .; __end = .;
}
