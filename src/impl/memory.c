#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "../ckit.h"

#define MAKER_SIZE 8

// #define TRACE_MEMORY(...) fprintf(stderr, __VA_ARGS__)
#define TRACE_MEMORY(...)

static const char BEG_CHECK[MAKER_SIZE] = "((BEG[[<";
static const char END_CHECK[MAKER_SIZE] = ">END]]))";

typedef struct
{
    uint8_t *ptr;
    size_t size;
    time_t allocationDate;
    unsigned used : 1;
} ckit_memory_block;

typedef struct
{
    ckit_memory_block *allocations;
    int count; // currently allocated slots
    int total; // the total number of allocations
} ckit_memory_info;

static ckit_memory_info infos = {NULL, 0, 0};

const ckit_memory_info *memory_infos(void)
{
    return &infos;
}

static ckit_memory_block *find_in_list(const void *ptr)
{
    ckit_memory_block *available_alloc = NULL;
    int i;
    for (i = 0; i < infos.count; i++)
    {
        if (infos.allocations[i].ptr == ptr)
        {
            return &infos.allocations[i];
        }
        else if (!infos.allocations[i].used)
        {
            available_alloc = &infos.allocations[i];
        }
    }
    // We create a new one...
    if (available_alloc == NULL)
    {
        infos.allocations = realloc(infos.allocations, sizeof(ckit_memory_block) * (infos.count + 1));
        available_alloc = &infos.allocations[infos.count++];
        TRACE_MEMORY("ckit::find_in_list(): Expanded the list to %i elements (ptr=%p)\n", infos.count, infos.allocations);
    }
    available_alloc->ptr = (uint8_t *)ptr;
    available_alloc->size = 0;
    available_alloc->used = 0;
    available_alloc->allocationDate = 0L;
    return available_alloc;
}

static void add_to_list(const void *ptr, int size)
{
    ckit_memory_block *alloc = find_in_list(ptr);
    assert(!alloc->used);
    alloc->ptr = (uint8_t *)ptr;
    alloc->used = 1;
    alloc->size = size;
    alloc->allocationDate = time(NULL);
}

static void remove_from_list(const void *ptr)
{
    ckit_memory_block *infos = find_in_list(ptr);
    infos->ptr = NULL;
    infos->used = 0;
    infos->size = 0;
    infos->allocationDate = 0;
}

static char HEXADEC[] = "0123456789ABCDEF";

void ckit_memory_dump(FILE *f, const void *ptr, size_t len)
{
    const uint8_t *dump = ptr;
    int i = 0;
    while (i < len)
    {
        int LINE_SIZE = 16;
        fprintf(f, "%p: ", dump);
        char hexa[LINE_SIZE * 3 + 1];
        char ascii[LINE_SIZE + 1];
        for (int j = 0; j < LINE_SIZE; j++)
        {
            if (i < len)
            {
                hexa[j * 3 + 0] = HEXADEC[dump[i] / 16];
                hexa[j * 3 + 1] = HEXADEC[dump[i] % 16];
                hexa[j * 3 + 2] = ' ';
                ascii[j] = (dump[i] < 32 || dump[i] > 126) ? '.' : dump[i];
            }
            else
            {
                hexa[j * 3] = hexa[j * 3 + 1] = hexa[j * 3 + 2] = ascii[j] = ' ';
            }
            i++;
        }
        hexa[LINE_SIZE * 3] = 0;
        ascii[LINE_SIZE] = 0;
        fprintf(f, "%s  %s\n", hexa, ascii);
    }
}

static void check_validity(const void *p, int size)
{
    const char *ptr = p;
    assert(sizeof(int64_t) == MAKER_SIZE);
    int begin = memcmp(ptr - MAKER_SIZE, BEG_CHECK, MAKER_SIZE);
    int end = memcmp(ptr + size, END_CHECK, MAKER_SIZE);
    if (begin != 0 || end != 0)
    {
        fprintf(stderr, "Memory issue at %p (size %i). Dump follows:\n", ptr, size);
        ckit_memory_dump(stderr, ptr - MAKER_SIZE, size + 2 * MAKER_SIZE);
        ckit_exit("Stopped due to damaged memory.");
    }
}

#define FILLER '*'

static uint8_t *debug_alloc(int size)
{
    int i;
    uint8_t *ptr = malloc(size + MAKER_SIZE * 2);
    memcpy(ptr, BEG_CHECK, MAKER_SIZE);
    ptr += MAKER_SIZE;
    memcpy(ptr + size, END_CHECK, MAKER_SIZE);
    add_to_list(ptr, size);
    infos.total++;
    memset(ptr, FILLER, size);
    ckit_validate_memory();
    TRACE_MEMORY("INFO: allocated %u bytes at %p\n", size, ptr);
    if(size == 1){
        ckit_backtrace();
    }
    return ptr;
}

static void debug_free(uint8_t *ptr)
{
    ckit_memory_block *alloc = find_in_list(ptr);
    if (!alloc->used)
    {
        char buf[100];
        sprintf(buf, "Try to free a bad pointer (%p)", ptr);
        ckit_exit(buf);
    }
    TRACE_MEMORY("INFO: freeing %zu bytes at %p...\n", alloc->size, ptr);
    check_validity(ptr, alloc->size);
    memset(ptr - MAKER_SIZE, 'x', alloc->size + 2 * MAKER_SIZE); // ensure data is wrapped.
    alloc->ptr = NULL;
    alloc->used = 0;
    alloc->size = 0;
    alloc->allocationDate = 0;
    free(ptr - MAKER_SIZE);
}

/**
 * The reallocation is quite long because, in any case,
 * the memory is allocated to receive a copy.
 */
static void *debug_realloc(uint8_t *ptr, size_t newsize)
{
    ckit_memory_block *infos = find_in_list(ptr);
    if (!infos->used)
    {
        ckit_exit("Pointer is illegal (not allocated or freed).");
    }
    size_t size = infos->size;
    check_validity(infos->ptr, infos->size);
    uint8_t *newptr = debug_alloc(newsize);
    check_validity(newptr, newsize);
ckit_validate_memory();
    if (newsize > infos->size)
    {
        TRACE_MEMORY("INFO: copying data + adding filler\n");
        memcpy(newptr, infos->ptr, infos->size);
        // memset(&newptr[infos->size], FILLER, newsize - infos->size);
    } else {
        TRACE_MEMORY("INFO: copying original data up to %zu bytes\n", newsize);
        memcpy(newptr, infos->ptr, newsize);
    }
    TRACE_MEMORY("INFO: reallocated %zu (%p) --> %zu (%p)\n", size, ptr, newsize, newptr);
    ckit_validate_memory();
    debug_free(ptr);
    return newptr;
}

void *ckit_alloc(size_t size)
{
    size = size < 1 ? 1 : size;
    uint8_t *ptr;
#ifdef CKIT_DEBUG
    ptr = debug_alloc(size);
#else
    ptr = malloc(size);
#endif
    if (ptr == NULL)
    {
        ckit_exit("Unable to allocate memory.");
    }
    return ptr;
}

void ckit_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
#ifdef CKIT_DEBUG
    debug_free(ptr);
#else
    free(ptr);
#endif
}

void *ckit_realloc(void *ptr, size_t newsize)
{
    void *new_ptr;
    if (ptr == NULL)
    {
        // We adhere to realloc contract.
        return ckit_alloc(newsize);
    }
    newsize = newsize < 1 ? 1 : newsize;
#ifdef CKIT_DEBUG
    new_ptr = debug_realloc(ptr, newsize);
#else
    new_ptr = realloc(ptr, newsize);
#endif
    if (new_ptr == NULL)
    {
        ckit_exit("Unable to reallocate memory.");
    }
    return new_ptr;
}

void ckit_memory_check(const void *ptr)
{
    ckit_memory_block *alloc = find_in_list(ptr);
    if (alloc == NULL || !alloc->used)
    {
        char buff[100];
        sprintf(buff, "Pointeur %p is not currently in use!", ptr);
        ckit_exit(buff);
    }
    check_validity(ptr, alloc->size);
}

int ckit_validate_memory()
{
    int checked = 0;
    int i = 0;
    for (i = 0; i < infos.count; i++)
    {
        if (infos.allocations[i].used)
        {
            check_validity(infos.allocations[i].ptr, infos.allocations[i].size);
            checked++;
        }
    }
    return checked;
}