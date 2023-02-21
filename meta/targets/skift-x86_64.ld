ENTRY(_start)

SECTIONS
{
	. = 4M;

	.text : ALIGN(4K)
	{
		*(.text)
	}

	.rodata : ALIGN(4K)
	{
		*(.rodata)

        __init_array_start = .;
        *(.init_array)
        __init_array_end = .;
	}

	.data : ALIGN(4K)
	{
		*(.data)
	}

	.bss : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
}
