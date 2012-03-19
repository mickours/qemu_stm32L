/* stm32.ld: Linker script for the STM32 controller series. */
/* This file is specifically for the STM32F100 on the Discovery board,
* with 128K/8K Flash/RAM
* Written 2010/2011 by Donald Becker
*/
/* The OUTPUT and ENTRY settings repeat the defaults. */
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
user_flash (rx) : ORIGIN = 0x08000000, LENGTH = 128K
system_flash (rx) : ORIGIN = 0x1ffff7ff, LENGTH = 2K
ram (rwx) : ORIGIN = 0x20000000, LENGTH = 8K
}

/* Stack starts at the top of RAM. This changes based on the RAM size,
* so we default to using the size on the smallest chip. */
_user_stack_start = 0x20002000;

SECTIONS
{
. = 0x08000000; /* Redundant w/ user_flash setting above. */
.text :
{
*(vectors) /* Interrupt vector table, 0..0x0133 */
*(.text) /* Program code */
*(.text.*) /* Specially marked code e.g. infrequently used. */
*(.rodata) /* Read only data / constant storage */
} >user_flash
. = ALIGN(4);
_initdata_flash = .; /* Used by _start() for initialized vars. */

. = 0x20000000; /* RAM from 0x20000000. Again, redundant w/ above. */
.data :
{
_initdata_start = ABSOLUTE(.); /* Used by _start() to memcpy */
*(.data) /* Initialized data memory */
_edata = . ;
_initdata_end = ABSOLUTE(.); /* Used by _start() to memcpy */
} >ram AT > user_flash /* Addresses linked as RAM, appended into flash. */

.bss (NOLOAD): /* Zero-filled static allocated data memory */
{
_bss_start = .; /* _bss_start and _bss_end used by crt0 */
*(.bss)
*(.bss.*)
. = ALIGN(4);
_bss_end = .;
} >ram AT > user_flash
/* Define "end" and "_end" only if the program doesn't define its own. */
PROVIDE ( end = _bss_end );
PROVIDE ( _end = _bss_end );
}

/* Link against the C Run Time which holds the call to main(). */
/* This embedded path is horribly bogus, but expedient. */
INPUT( ../armduino/crt-stm32.o )


