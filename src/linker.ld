ENTRY(_start)  /*Entry point*/
OUTPUT_FORMAT(binary)
SECTIONS
{
	. = 1M; /*We load 1 megabite of the kernel into memory*/
	.text ALIGN(4096):
	{
		*(.text)
	}

	
	.rodata : ALIGN(4096)
	{
		*(.rodata)
	}

	.data : ALIGN(4096)
	{
		*(.data)
	}	

	.bss : ALIGN(4096) /*Uninitielized variables at the start*/
	{
		*(COMMON)
		*(.bss)
	}

	.asm : ALIGN(4096)
	{
		*(.asm)
	}
}

