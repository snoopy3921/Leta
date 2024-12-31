ENTRY(Reset_Handler)

MEMORY
{
    FLASH(rx)	: ORIGIN = 0x08000000, LENGTH = 64K
    SRAM(rwx)   : ORIGIN = 0x20000000, LENGTH = 16K
}

_sram   =  ORIGIN(SRAM);    


/* heap size */
HEAP_SIZE = 5K;


SECTIONS
{
    .text :
    {        
		_isr_vector = .;
        KEEP(*(.isr_vector))
        *(.text)
        *(.rodata)
        *(.glue_7) /* glue arm to thumb */
		*(.glue_7t)/* glue thumb to arm */
		KEEP (*(.init))
		KEEP (*(.fini))
        . = ALIGN(4);
    } > FLASH

	/* constructor data for object C++ */

	.preinit_array : {
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(SORT(.preinit_array.*)))
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} > FLASH

	.init_array : {
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);
	} > FLASH

	.fini_array : {
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(.fini_array*))
		KEEP (*(SORT(.fini_array.*)))
		PROVIDE_HIDDEN (__fini_array_end = .);
	} > FLASH

	/* end of FLASH */
	.end_flash : {
        . = ALIGN(4);	
        _end_flash = .;
	} > FLASH

    .data : AT(_end_flash)
    {
        _data = .;
        _ldata = LOADADDR(.data);
		*(vtable)
		*(.data)
		*(.data*)
        . = ALIGN(4);	
        _edata = .;
    } > SRAM

	.bss : {
		_bss = .;
		*(.bss)
		*(.bss*)
		_ebss = .;
	} > SRAM


    /* Check if size of heap and stack is fit in ram */
    .heap :
    {
        _heap_start = .;    /* define a global symbol at heap start */
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = ALIGN(4);
        . = . + HEAP_SIZE;
    } >SRAM

    _estack =  ORIGIN(SRAM) + LENGTH(SRAM);    /* end of RAM */

    /DISCARD/ : {
		libc.a ( * )
		libm.a ( * )
		libg.a ( * )
		libgcc.a ( * )
	}
    
}