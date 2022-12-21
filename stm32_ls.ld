/*The first instruction run of the programme is that inside the ENTRY*/
ENTRY(Reset_Handler)

/*format of name [(attr)] : ORIGIN = origin, LENGTH = len*/
/*This command describes the location and size of memory in target*/
MEMORY
{
	ROM(rx):ORIGIN =0x08000000,LENGTH =1024K
	SRAM(rwx):ORIGIN =0x20000000,LENGTH =128K
}

/*The SECTIONS command tells the linker how to map input sections into output sections, and how to place the output sections in memory.*/
SECTIONS
{
	/* the * around .text denotes the inclusion of .text from all input files*/
	.text :
	{
		*(.isr_vector)
		*(.text)
		*(.rodata)
		/*The value of location counter is aligned to a word value*/
		. = ALIGN(4);
		/*the "." symbol also known as linker script symbol defines the size of the section. It is incremented for every added output*/
		_etext = .;
	
	}> ROM 
	
	.data :
	{
		/*linker symbol always tracks vma so the linker symbol points to the start of SRAM*/
		_sdata = .;
		*(.data)
		. = ALIGN(4);
		_edata = .;
	/*First provided is VMA then LMA*/
	}>SRAM AT> ROM
	
	.bss :
	{
		_sbss = .;
		*(.bss)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .; 
	/* bss only exists in SRAM as uinitalized originally */
	} >SRAM
}
