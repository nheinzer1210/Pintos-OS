#include "threads/loader.h"

/* 
 * The linker script used to link the kernel. Sets the load address of the
 * kernel and arranges for "start.S" to be near the beginning of the kernel
 * image. Again you should not need to look at this code or modify it but it's
 * here for curosity. MARTIN
*/

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(start)			/* Kernel starts at "start" symbol. */
SECTIONS
{
  /* Specify the kernel base address. */
  _start = LOADER_PHYS_BASE + LOADER_KERN_BASE;

  /* Make room for the ELF headers. */
  . = _start + SIZEOF_HEADERS;

  /* Kernel starts with code, followed by read-only data and writable data. */
  .text : { *(.start) *(.text) } = 0x90
  .rodata : { *(.rodata) *(.rodata.*) 
	      . = ALIGN(0x1000); 
	      _end_kernel_text = .; }
  .data : { *(.data) 
	    _signature = .; LONG(0xaa55aa55) }

  /* BSS (zero-initialized data) is after everything else. */
  _start_bss = .;
  .bss : { *(.bss) }
  _end_bss = .;

  _end = .;

  ASSERT (_end - _start <= 512K, "Kernel image is too big.")
}
