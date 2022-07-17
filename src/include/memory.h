
#ifndef MEMORY_H__
#define MEMORY_H__

extern void *ckit_alloc(size_t size);
extern void ckit_free(void *);
extern void *ckit_realloc(void *, size_t newsize);

/**
 * @brief make an hexadecimal dump on the specified file.
 * 
 * Display 16 bytes per line, very basic but useful to check part of
 * memory.
 * 
 * @param out the file where to write (usually "stderr" or "stdout").
 * @param ptr the start of the pointer.
 * @param size the size in bytes.
 */
extern void ckit_memory_dump(FILE *out, const void *ptr, size_t size);

/**
 * Check a pointer allocated by ckit_alloc() or ckit_realloc. The pointer MUST not be
 * freed.
 * 
 */
extern void ckit_memory_check(const void *ptr);

/**
 * Chack all the current pointers. This method works only on DEBUG mode
 * and returns the number of memory blocks checked.
 */
extern int ckit_validate_memory();

#endif
