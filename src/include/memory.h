
#ifndef MEMORY_H__
#define MEMORY_H__

extern void *ckit_alloc(size_t size);
extern void *ckit_calloc(size_t nmemb, size_t size);
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

#endif
