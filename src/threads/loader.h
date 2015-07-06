#ifndef THREADS_LOADER_H
#define THREADS_LOADER_H
/* 
 * The Kernel loader. Assembles to 512 bytes of code and data that the PC
 * BIOS loads into memory and which in turn finds the kernel on disk, loads
 * it into memory, and jumps to start() in "start.S", You should not need to
 * look at this code or modify it.
 * 
 * -------------------------A1.1 THE LOADER----------------------------------
 * 
 * The first part of Pintos that runs is the loader in "threads/loader.S" The
 * PC BIOS loads the loader into memory. The loader, in turn, is responsible 
 * for finding the kernel on disk, loading it into memory, and then jumping to 
 * its start. 
 * 
 * ***************************************************************************
 * 
 * HOW THE LOADER LOAD: The PC BIOS loads the loader from the first sector of
 * the first hard disk, called the master boot record (MBR). PC conventions
 * reserve 64 bytes of the MBR for the partition table and Pintos uses about 
 * 128 additional bytes for kernel command-line arguments. This leaves about 
 * 300 bytes for the loaders own code. This is a severe restriction that means, 
 * practically speaking, the loader must be written in Assembly.
 *
 * The Pintos loader and kernel don't have to be on the same disk, nor is the
 * kernel required to be in any particular location on a given disk. The
 * loader's first job then is to find the kernel by reading the partition table
 * on each hard disk, looking for a bootable partition of the type used for a 
 * Pintos kernel.
 *
 * When the loader finds a bootable kernel partition, it reads the partition's
 * contents into memory at a physical address 128 KB. The kernel is at the
 * beginning of the partition which might be larger than necessary due to
 * partition boundary alignment conventions, so the loader reads no more than
 * 512 KB (and the Pintos build process will refuse to produce kernels larger
 * than that). Reading more data than this would crosss into the region from 640
 * KB to 1 MB that the PC architecture reserves fro hardware and the BIOS, and a
 * standard PC BIOS does not provide any means to lead the kernel above 1 MB. 
 *
 * The loader's final job is to extract the entry point from the loaded kernel
 * image and transfer control to it. The enry point is not a predictable
 * location, but the kernel's ELF header contains a pointer to it. The loader
 * extracts the pointer and jumps to the location it points to. 
 *
 * The Pintos kernel command line is stored in the boot loader, the pinto
 * program actualy modifies a copy of the boot loader on disk each time it runs
 * the kernel, inserting whatever command-line arguments the user supplies to
 * the kernel at boot time. The kernel at boot time reads those arguments out of
 * the boot loader into memory. This is not an elegant solution but it is simple
 * and effective. 
 *
 * MARTIN FAHY
*/


/* Constants fixed by the PC BIOS. */
#define LOADER_BASE 0x7c00      /* Physical address of loader's base. */
#define LOADER_END  0x7e00      /* Physical address of end of loader. */

/* Physical address of kernel base. */
#define LOADER_KERN_BASE 0x20000       /* 128 kB. */

/* Kernel virtual address at which all physical memory is mapped.
   Must be aligned on a 4 MB boundary. */
#define LOADER_PHYS_BASE 0xc0000000     /* 3 GB. */

/* Important loader physical addresses. */
#define LOADER_SIG (LOADER_END - LOADER_SIG_LEN)   /* 0xaa55 BIOS signature. */
#define LOADER_PARTS (LOADER_SIG - LOADER_PARTS_LEN)     /* Partition table. */
#define LOADER_ARGS (LOADER_PARTS - LOADER_ARGS_LEN)   /* Command-line args. */
#define LOADER_ARG_CNT (LOADER_ARGS - LOADER_ARG_CNT_LEN) /* Number of args. */

/* Sizes of loader data structures. */
#define LOADER_SIG_LEN 2
#define LOADER_PARTS_LEN 64
#define LOADER_ARGS_LEN 128
#define LOADER_ARG_CNT_LEN 4

/* GDT selectors defined by loader.
   More selectors are defined by userprog/gdt.h. */
#define SEL_NULL        0x00    /* Null selector. */
#define SEL_KCSEG       0x08    /* Kernel code selector. */
#define SEL_KDSEG       0x10    /* Kernel data selector. */

#ifndef __ASSEMBLER__
#include <stdint.h>

/* Amount of physical memory, in 4 kB pages. */
extern uint32_t init_ram_pages;
#endif

#endif /* threads/loader.h */
