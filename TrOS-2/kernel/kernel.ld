ENTRY(kernel_init)
SECTIONS
{
    . = 0xC0000000;
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
        *(.kernel_stack)
    }

    /DISCARD/ :
    {
        *(.eh_frame)
    }

    end = .; _end = .; __end = .;
}
