#ifndef THREADS_PALLOC_H
#define THREADS_PALLOC_H

#include <stddef.h>

/* How to allocate pages. */
enum palloc_flags
  {
    PAL_ASSERT = 001,           /* Panic on failure. */
    PAL_ZERO = 002,             /* Zero page contents. */
    PAL_USER = 004              /* User page. */
  };

/*
 * palloc_get_multiple:
 * 
 * Obtains and returns one page, or page_cnt contigous pages, respectively.
 * Returns a null pointer if the pages cannot be allocated. The flags argument
 * may be any combination of the following flags: 
 * 
 * PAL_ASSERT: 
 * 
 * If the pages cannot be allocated, panic the kernel. This is only appropriate
 * during kernel initialization. User process should never be permitted to panic
 * the kernel. 
 *
 * PAL_ZERO: 
 *
 * Zero all the bytes in the allocated pages before returning them. If not set,
 * the contents of newly allocated pages are unpredictable. 
 *
 * PAL_USER: 
 *
 * Obtain the pages from the user pool If not set, are allocated from the kernel
 * pool. 
 *
 * palloc_free_multiple: 
 *
 * Frees one page, or page_cnt contiguous pages, respectively, starting at
 * pages. All of the pages must have been obtained using palloc_get_page() or
 * palloc_get_multiple(). 
*/ 

void palloc_init (size_t user_page_limit);
void *palloc_get_page (enum palloc_flags);
void *palloc_get_multiple (enum palloc_flags, size_t page_cnt);
void palloc_free_page (void *);
void palloc_free_multiple (void *, size_t page_cnt);

#endif /* threads/palloc.h */
