

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "../ckit.h"

DEFINE_CLASS(CMemory) {
    uint8_t *buf;
    size_t used;
    size_t size;
};

#define WCHAR_SIZE siezof(wchar_t)
#define MARKER "<END>"

/**
 * @brief if debug is active, then checks if the memory has not been written.
 * 
 */
#define CHECK_OVERRUN(p) assert(strcmp((const char *)&self->buf[self->used], MARKER) == 0)

static inline void set_length(CMemory *self, size_t used){
    self->used = used;
#ifndef NDEBUG
    strcpy((char *)&self->buf[self->used], MARKER);
#endif
}

ALLOCATE_DEFINITION(CMemory)
{
    self->size = 16;
    self->buf = ckit_alloc(self->size);
    set_length(self, 0);
}

DISPOSE_DEFINITION(CMemory) {
    CHECK_OVERRUN(self);
    ckit_free(self->buf);
}

REGISTER_CLASS(CMemory)

static void ensure_size(CMemory *self, size_t minsize){
    if(minsize > self->size){
        self->size = minsize + 5 * sizeof(int);
        self->buf = ckit_realloc(self->buf, self->size + 8);
    }
}

void cmemory_append(CMemory *self, const void *ptr, size_t len){
    CHECK_OVERRUN(self);
    // printf("**** %s ****\n", (const char *)ptr);
    ensure_size(self, self->used + len);
    memcpy(&self->buf[self->used], ptr, len);
    set_length(self, self->used + len);
}

uint8_t* cmemory_buffer(CMemory *self, int size){
    CHECK_OVERRUN(self);
    if(size >= 0){
        ensure_size(self, size);
        if(size > self->used){
            memset(&self->buf[self->used], 0, size - self->used);
        }
        set_length(self, size);
    }
    return self->buf;
}

size_t cmemory_length(const CMemory *self){
    CHECK_OVERRUN(self);
    return self->used;
}
    
void cmemory_dump(const CMemory *self, FILE *f){
    CHECK_OVERRUN(self);
    ckit_memory_dump(f, self->buf, self->used);
}


