#ifndef _CKIT_MEMORY_H__
#define _CKIT_MEMORY_H__

CLASS_DECLARE(CMemory)

/**
 * @brief append memory to the current buffer
 * 
 * @param self the buffer.
 * @param ptr the pointer
 * @param len the length to add
 */
extern void cmemory_append(CMemory *self, const void *ptr, size_t len);
extern uint8_t*cmemory_buffer(CMemory *self, int size);

/**
 * @brief dump the memory to a stream (usually "stdout")
 * 
 * @param self the memory to dump
 * @param f the output stream
 */
extern void cmemory_dump(const CMemory *self, FILE *f);

#endif