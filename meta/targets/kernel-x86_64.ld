ENTRY(_kstart)

SECTIONS
{
    . = 0xffffffff80200000;

    .handover : ALIGN(4K)
    {
        KEEP(*(.handover*))
    }

    .text : ALIGN(4K)
    {
        *(.text*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata*)

        __init_array_start = .;
        *(.init_array)
        __init_array_end = .;
    }

    .data : ALIGN(4K)
    {
        *(.data*)
    }

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss*)
    }
}
