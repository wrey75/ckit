#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "../ckit.h"

#define MAKER_SIZE 8
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

static ckit_memory_block *find_in_list(void *ptr)
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
    }
    available_alloc->used = 0;
    return available_alloc;
}

static void add_to_list(void *ptr, int size)
{
    ckit_memory_block *alloc = find_in_list(ptr);
    alloc->ptr = ptr;
    alloc->used = 1;
    alloc->size = size;
    if (alloc->allocationDate == 0)
    {
        alloc->allocationDate = time(NULL);
    }
}

static void remove_from_list(void *ptr)
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
    // int i;
    // for(i=0; i < MAKER_SIZE; i++){
    //     if(ptr[i - MAKER_SIZE] == BEGIN_CHECK[i] || ptr[i + size] == END_CHECK[i]){
    //        ckit_exit("Overridden memory.");
    //    }
    //}
    const char *ptr = p;
    assert(sizeof(int64_t) == MAKER_SIZE);
    int begin = memcmp(ptr - MAKER_SIZE, BEG_CHECK, MAKER_SIZE);
    int end = memcmp(ptr + size, END_CHECK, MAKER_SIZE);
    if (begin != 0 || end != 0)
    {
        ckit_memory_dump(stderr, ptr - MAKER_SIZE, size + 2 * MAKER_SIZE);
        ckit_exit("Overridden memory (see dump above).");
    }
}

static uint8_t *debug_alloc(int size)
{
    int i;
    uint8_t *ptr = malloc(size + MAKER_SIZE * 2);
    memcpy(ptr, BEG_CHECK, MAKER_SIZE);
    ptr += MAKER_SIZE;
    memcpy(ptr + size, END_CHECK, MAKER_SIZE);
    add_to_list(ptr, size);
    infos.total++;
    fprintf(stderr, "INFO: alloc %u bytes at %p\n", size, ptr);
    // hexa_dump(stderr, ptr - MAKER_SIZE, size + MAKER_SIZE * 2);
    check_validity(ptr, size);
    return ptr;
}

static void debug_free(uint8_t *ptr)
{
    ckit_memory_block *alloc = find_in_list(ptr);
    if (!alloc->used)
    {
        ckit_exit("Try to free a bad pointer.");
    }
    fprintf(stderr, "INFO: freed %zu bytes at %p\n", alloc->size, ptr);
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
        ckit_exit("Try to reallocate memory an used pointer.");
    }
    size_t size = infos->size;
    check_validity(infos->ptr, infos->size);
    uint8_t *newptr = debug_alloc(newsize);
    memcpy(newptr, infos->ptr, infos->size);
    fprintf(stderr, "INFO: realloc %zu (%p) --> %zu (%p)\n", size, ptr, newsize, newptr);
    debug_free(ptr);
    // infos->size = newsize;
    // infos->ptr = newptr;
    // infos->used = 1; // because the block is freed.
    return newptr;
}

void *ckit_alloc(size_t size)
{
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
