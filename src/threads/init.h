#ifndef THREADS_INIT_H
#define THREADS_INIT_H

#include <debug.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 
 * Kernel initialization including main(), the kernel's "main program." You
 * should look over main() at least to see what gets initialized. You may want
 * to add own initialization code here. 
 * 
 * ------------------------HIGH-LEVEL KERNEL INITIALIZATION---------------------
 *  
 *  The kernel proper starts with the main() function. The main() function is
 *  written in C. When main() starts, the system is in a pretty raw state. We're
 *  in 32-bit protected mode with paging enabled, but hardly anything else is
 *  ready. Thus the main() function consists primarily of calls into other
 *  Pintos modules' initialization functions. These are usually named
 *  module_init(), where module is the module's name, "module.c" is the modules
 *  source code and "module.h" is the modules header. 
 *
 *  The first step in main() is to call bss_init(), which clears out the
 *  kernels "BSS", which is the traditional name for a segment that should be
 *  initialized to all zeros. In most C implementations, whenever you declare a
 *  variable outside a function without providing an initializer, that variaible
 *  goes ino the BSS. Because its all zeros, the BSS isnt stored in the image
 *  that the loader brought into memory. We just memset() to zero it out. 
 *
 *  Next main() calls read_command_line() to break the kernel command line into
 *  arguments, then parse_options() to read and options at the beginning of the
 *  command line. (Actions specified on the command line execute later.) 
 *
 *  thread_init() initializes the thread system. We will defer full discussion
 *  to our discussion of Pintos threads below. It is called so early in
 *  initialization because a valid thread stucture is a prerequisite for
 *  acquiring a lock, and lock acquisition in turn is important to other Pintos
 *  subsystems. The we initialze the console to print a startup message to the
 *  console. 
 *  
 *  The next block of functions we call initializes the kernel's memory system.
 *  palloc_init() sets up the kernel page allocator, which doles out memory one
 *  or more pages at a time 
 *
 *  -->
 *
 *  A 5.1 PAGE ALLOCATOR:
 *  
 *  The page allocator declared in "threads/polloc.h" allocates memory in units
 *  of a page. It is most often used to allocate memory one page at a time, but
 *  it can also allocate multiple contigous pages at once. 
 *
 * The page allocator divides the memory it allocates into two pools, called the
 * kernel and user pools. By default each pool gets half of system memory above
 * 1MB but the division can be changed with the "-ul" kernel command line
 * option. An allocation request draws from one pool of the other. If one pool
 * becomes empty, the other may still have free pages. The user pool should be
 * used for allocating memory for user processes and the kernel pool for all
 * other allocations. This will only become important starting with project 3.
 * Until then, all allocations should be made from the kernel pool. 
 *
 * Each pool's usage is tracked with a bitmap, one bit per page in the pool. A
 * request to allocate n pages scans the bitmap for n consecutive bits set to
 * false, indicating that those pages are free, and then sets those bits to true
 * to mark them as used. This is a "first fit allocation strategy." 
 *
 * The page allocator is subject to fragmentation. That is, it may not be
 * possible to allocate n contigous pages even though n or more pages are free,
 * because the free pages are separated by used pages. Int fact, in pathological
 * cases it may be impossible to allocate 2 contigous pages even though half of
 * the pool's pages are free. Single-page requests can't fail due to
 * fragmentation, so requests for multiple contiguous pages should be limited as
 * much as possible. Pages may not be allocated from interrupt context, but they
 * may be freed. When a page is freed, all of it's bytes are cleared to )xcc as
 * a debugging aid. 
 * 
 * <--
 * 
 * malloc_init() setups up the allocator that handles allocations of
 * arbitrary-size blocks of memory 
 *
 * --> 
 *
 * A 5.2 BLOCK ALLOCATOR:
 *
 * The block allocator, declaed in "threads/malloc.h", can allocate blocks of
 * any size. It is layered on top of the page allocator described above. Blocks
 * returned by the block allocator are obtained from the kernel pool. 
 *
 * The block allocator uses two different strategies for allocating memory. The
 * first strategy applies to blocks that are 1KB or smaller (One-Fourth of the
 * page size). These allocations are rounded up to the nearest power of 2 or 16
 * bytes, whichever is larger. Then they are grouped into a page used only for
 * allocations of that size. 
 *
 * The second strategy applies to blocks larger than 1KB. These allocations
 * (plus a small amount of overhead) are rounded up to the nearest page in size,
 * and the block allocator requests that number of contigous pages from the page
 * allocator. 
 *
 * In either case, the difference between the allocation requested size and the
 * actual block size is wasted. A real operating system would carefully tune its
 * allocator to minimize this waste, but this is unimportant in an instructional
 * system like Pintos. 
 *
 * As long as a page can be obtained from the page allocator, small allocations
 * always succeed. Most small allocations do not require a new page from the
 * page allocator at all, because they are satisfied using part of a page
 * already allocated. However, large allocations always require calling into the
 * page allocator, and any allocation that needs more than one contigous page
 * can fail due to fragmentation, as already discussed in the previous section.
 * Thus you should minimize the number of large allocations in your code,
 * especially those over approximately 4 KB each. When a block is freedm all of
 * its bytes are cleared to 0xcc as a debugging aid. The block allocator may not
 * be called from interrupt context. 
 *
 * <--
 * paging_init() sets up a page table for the kernel
 *
 * --> 
 *
 * A.7 PAGE TABLE: 
 * 
 * The code in "pagedir.c" is an abstract interface to the 80x86 hardware page
 * table, also called a "page directory" by Intel processor documentation. The
 * page table interface uses a uint32_t * to represent a page table because this
 * is convenient for accessing their internal structure
 *
 * <--
 * In projects 2 and later, main() also calls tss_init() andgdt_init(). 
 *
 * The next set of calls initializes the interrupt system. intr_init() sets up
 * the CPU's interrupt descriptor table (IDT) to ready it for interrupt
 * handling. 
 *
 * --> 
 *
 *  A 4.1 INTERRUPT INFRASTRUCTURE: 
 *
 *  When an interrupt occurs, the CPU saves its most essential state on a stack
 *  and jumps to an interrupt handler routine. The 80x86 architecture supports
 *  256 interrupts, numbered 0 through 255, each with an independent handler
 *  defined in an array called the interrupt descriptor table or IDT. 
 *
 *  In Pintos, intr_init() in "threads/interrupt.c" sets up the IDT so that each
 *  entry points to a unique entry point in "threads/int-stubs.S" named
 *  intNN_stub(), where NN is the interrupt number in hexadecimal. because the
 *  CPU doesn't give us any other way to find out the interrupt number, this
 *  entry point pushes the interrupt number on the stack. Then it jumps to
 *  intr_entry(), which pushes all the registers that the processor didn't
 *  already push for us, and then calls intr_handler(), which brings us back
 *  into C in "threads/interrupt.c". 
 *
 *  The main job of int_handler() is to call the function registered for
 *  handling the particular interrupt (If no function is registered, it dumps
 *  some information to the console and panics.) It also does some extra
 *  processing for external interrupts. 
 *
 *  --> 
 *
 *  A 4.3 EXTERNAL INTERRUPT HANDLING:
 *
 *  External interrupts are caused by events outside the CPU. They are
 *  asynchronous, so they can be invoked at any time that interrupts have not
 *  been disabled. We say that an external interrupt runs in an "interrupt
 *  context." 
 *
 *  In an external interrupt, the struct intr_fame passed to the handler is not
 *  very meaningful. It describes the state of the thread or process that was
 *  interrupted, but there is no way to predict which one that is. It is
 *  possible, although rarely useful, to examine it, but modifying it is a recipe
 *  for distaster. 
 *
 *  An external interrupt handler must not sleep or yield, which rules out
 *  calling lock_acquire(), thread_yield(), and many other functions. Sleeping
 *  in interrupt context would effectively put the interrupted thread to sleep,
 *  too, until the interrupt handler was again scheduled and returned. This
 *  would be unfair to the unlucky thread, and it would deadlock if the handler
 *  were waiting for the sleeping thread to release a lock. 
 *
 *  An external interrupt handler effectively monopolizes the machine and delays
 *  all other activities. Therefore, external interrupt handlers should complete
 *  as quickly as they can. Anything that requires much CPU time should instead
 *  run in a kernel thread, possibly one that the interrupt triggers using a
 *  synchronization primitive. 
 *
 *  External interrupts are controlled by a pair of devices outside the CPU
 *  called programmable interrupt controllers, PICs for short. When intr_init()
 *  sets up the CPU's IDT, it also initializes thePICs for interrupt handling.
 *  The PICs also must be "acknowledged" at the end of processing for each
 *  external interrupt. intr_handler() takes care of that by calling
 *  pic_end_of_interrupt(), which properly signals the PICs. 
 *
 *  <-- 
 *
 *  When intr_handler() returns, the assembly code in "threads/intr-stubs.S"
 *  restores all the CPU registers saved earlier and directs the CPU to return
 *  from the interrupt. 
 *
 *  A 4.2 INTERNAL INTERRUPT HANDLING: 
 *  
 * Internal interrupts are caused directly by CPU instructions exectued by the
 * running kernel thread or user process (from project 2 onward). An internal
 * interrupt is therefore said to arise in a "process context." 
 *
 * In an internal interrupts handler, it can make sense to examine the struct
 * intr_frame passed to the interrupt handler, or even to modify it. When the
 * interrupt returns modifications in struct intr_frame become changes to the
 * calling thread or process's state. For example, the Pintos system call
 * handler returns a calue to the user pogram by modifying the saved EAX
 * register. 
 *
 * There are no special restrictions on what an internal interrupt handler can
 * or cant do. Generally they shoudl run with interrupts enabled, just like
 * other code, and so they can be preempted by other kernel threads. Thus they
 * do need to synchronize with other threads on shared data and other resources.
 *
 * Internal interrupt handlers can be invoked recursively. For example, the
 * system call handler might cause a page fault while attempting to read user
 * memory. Deep recursion would risk overflowing the limited kernel stack but
 * should be unnecessary. 
 *
 * <-- 
 *
 * Then timer_init() and kbd_init() prepare for handling timer interrupts and
 * keyboard interrupts, respectively. input_init() sets up to merge serial and
 * keyboard input into one stream. In projects 2 and later, we also prepare to
 * handle interrupts caused by user programs using exception_init() and
 * syscall_init() . 
 *
 * Now that interrupts are setup we can start the scheduler with thread_start(),
 * which creates the idle thread and enables interrupts. With interrupts
 * enabled, interrupt-dricen serial port I/O becomes possible, so we use
 * serial_init_queue() to switch to that mode. Finall, timer_calibrate()
 * calibrates the timer for accurate short delays. 
 *
 * IF the file system is complied in, as it will starting in Project2, we
 * initialize the IDE disks with ide_init(), the the file system with
 * filesys_init(). Boot is complete so we print a message. Function
 * run_actions() now parses and executes actions specified on the kernel command
 * line, such as run to run a test or a user program. 
 *
 * Finall if "-q" was specified on the kernel command line, we call
 * shutdown_power_off() to terminate the machine simulator. Otherwise, main()
 * calls thread_exit(), which allows any other running threads to continue
 * runnning. 
 *
 * MARTIN
*/ 

/* Page directory with kernel mappings only. */
extern uint32_t *init_page_dir;

#endif /* threads/init.h */
