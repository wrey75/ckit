/**
 *  Fast array implementation.
 * 
 * This implementation is quite basic: we just want to use a allocation for continuous memory
 * but we want to save the size of the original block AND we do not want random data in the
 * data newly created.
 * 
 * This really protect the memory and works more or like the object "Array" but only
 * provide C-fashion function. Mainly used internally to spped up the stuff and does
 * not requires user objects to do the job. Note we are in a low-level of abstraction.
 * 
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "../ckit.h"

// #define TRACE(...) fprintf(stderr, __VA_ARGS__)
#define TRACE(...)

#define MAGIC_NUMBER "_ckit_array_"


typedef struct {
    size_t element_size;
    size_t array_size;
    const char *magic_number;
} ckit_array_definition;

static inline ckit_array_definition * CHECK_MAGIC_NUMBER(void *ptr) {
    assert(ptr != NULL);
    ckit_array_definition *data = &((ckit_array_definition *)ptr)[-1];
    if(data->magic_number != MAGIC_NUMBER){
        ckit_exit("The pointer is not an ckit array.");
    }
    return data;
}

void *ckit_array_alloc(size_t count, size_t element_size) {
    size_t size = count * element_size;
    ckit_array_definition * ptr = (ckit_array_definition *)ckit_alloc(size + sizeof(ckit_array_definition));
    ptr->element_size = element_size;
    ptr->array_size = count;
    ptr->magic_number = MAGIC_NUMBER;
    ++ptr; // points to the user space
    memset(ptr, 0, size);
    TRACE("ckit_array_alloc(): allocated %zu bytes at %p\n", size, ptr);
    // ckit_memory_dump(stderr, &ptr[-1], sizeof(ckit_array_definition) + size);
    return ptr;
}

void *ckit_array_realloc(void *ptr, size_t newsize) {
    assert(ptr != NULL);
    ckit_array_definition * data = CHECK_MAGIC_NUMBER(ptr);
    size_t size = newsize * data->element_size;
    TRACE("ckit_array_realloc(), reallocates %zu entries (from %p).\n", newsize, ptr);
    data = ckit_realloc(data, sizeof(ckit_array_definition) + size);
    int len = data->element_size * (newsize - data->array_size);
    if(len > 0){
        uint8_t *from = ((uint8_t *)&data[1]) + data->element_size * data->array_size;
        memset(from, 0, len);
    }
    ckit_validate_memory();
    data->array_size = newsize;
    return &data[1];
}

void ckit_array_free(void *ptr) {
    ckit_array_definition *data = CHECK_MAGIC_NUMBER(ptr);
    ckit_free(data);
    TRACE("ckit_array_free(): ptr %p deleted.\n", ptr);
}

size_t ckit_array_size(void *ptr) {
    ckit_array_definition *data = CHECK_MAGIC_NUMBER(ptr);
    return data->array_size;
}


void *ckit_magic_alloc(size_t size, const char *magic) {
    const char **ptr = ckit_alloc(size + sizeof(char *));
    ptr[0] = magic;
    return (void *)&ptr[1];
}

void ckit_magic_free(void* ptr, const char *magic) {
    const char **data = (const char **)ptr;
    if(data[-1] != magic){
        ckit_exit("This pointer is unexpected.");
    }
    ckit_free(&data[-1]);
}