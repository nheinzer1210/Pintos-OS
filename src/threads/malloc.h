#ifndef THREADS_MALLOC_H
#define THREADS_MALLOC_H

#include <debug.h>
#include <stddef.h>

/*
 * void * malloc(): 
 *
 * Obtains and returns a new block, form the kerenl ppol, at least size bytes
 * long. Returns a null pointer is size is zero or if memory is not available. 
 *
 * void* calloc():
 *
 * Obtains a returns a new block, from the kernel pool, at least a * b bytes
 * long. The block's contents will be cleared to zeros. Returns a null pointer
 * if a or b is zero if insufficient memory is available. 
 *
 * void *realloc():
 *
 * Attempts to resize block to new_size bytes, possibly moving it in the
 * process. If successful, returns the new block, in which case the old block
 * must no longer be accessed. On failure, returns a null pointer, and the old
 * block remains valid. 
 *
 * A call with block null is equivalent to malloc(). A call with new_size zero
 * is equivalent to free() 
 *
 * void free():
 *
 * Frees block, which must have been previously returned by malloc, calloc, or
 * realloc. 
*/ 

void malloc_init (void);
void *malloc (size_t) __attribute__ ((malloc));
void *calloc (size_t, size_t) __attribute__ ((malloc));
void *realloc (void *, size_t);
void free (void *);

#endif /* threads/malloc.h */
