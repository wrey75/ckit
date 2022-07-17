/**
 * 
 */
#ifndef CKIT__FASTARRAY_H
#define CKIT__FASTARRAY_H

#define ARRAY_ALLOC(type, size) (type)ckit_array_alloc(sizeof(type), size)

extern void *ckit_array_alloc(size_t element_size, size_t array_size);
extern void *ckit_array_realloc(void *ptr, size_t array_size);
extern void ckit_array_free(void *ptr);
extern size_t ckit_array_size(void *ptr);

#endif