ENTRY(_kstart)

SECTIONS
{
	. = 1M;
	__start = .;

	.multiboot1 ALIGN(4K) :
	{
		KEEP(*(.multiboot1))
	}

	.multiboot2 ALIGN(4K) :
	{
		KEEP(*(.multiboot2))
	}

	.stivalehdr ALIGN(4K) :
	{
		KEEP(*(.stivalehdr))
	}

	.stivale2hdr ALIGN(4K) :
	{
		KEEP(*(.stivale2hdr))
	}

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	__end = .;
}
