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
#include "../ckit.h"

#define MAGIC_NUMBER "_ckit_array_"

typedef struct {
    size_t element_size;
    size_t array_size;
    const char *magic_number;
} ckit_array_definition;

static inline ckit_array_definition * CHECK_MAGIC_NUMBER(void *ptr) {
    ckit_array_definition *data = &((ckit_array_definition *)ptr)[-1];
    if(data->magic_number != MAGIC_NUMBER){
        ckit_exit("The pointer is not an ckit array.");
    }
    return data;
}

void *ckit_array_alloc(size_t element_size, size_t array_size) {
    size_t size = array_size * element_size;
    ckit_array_definition * ptr = (ckit_array_definition *)ckit_alloc(sizeof(ckit_array_definition) + size);
    ptr->element_size = element_size;
    ptr->array_size = array_size;
    ptr->magic_number = MAGIC_NUMBER;
    ++ptr; // points to the user space
    memset(ptr, 0, size);
    return ptr;
}

void *ckit_array_realloc(void *ptr, size_t newsize) {
    ckit_array_definition * data = CHECK_MAGIC_NUMBER(ptr);
    size_t size = newsize * data->element_size;
    data = ckit_realloc(data, sizeof(ckit_array_definition) + size);
    int len = data->element_size * (newsize - data->array_size);
    if(len >0){
        uint8_t *from = ((uint8_t *)&data[1]) + data->element_size * data->array_size;
        memset(from, 0, len);
    }
    data->array_size = newsize;
    return &data[1];
}

void ckit_array_free(void *ptr) {
    ckit_array_definition *data = CHECK_MAGIC_NUMBER(ptr);
    ckit_free(data);
}

size_t ckit_array_size(void *ptr) {
    ckit_array_definition *data = CHECK_MAGIC_NUMBER(ptr);
    return data->array_size;
}
