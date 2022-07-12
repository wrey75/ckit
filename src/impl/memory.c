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

typedef struct {
    uint8_t *ptr;
    int size;
    time_t allocationDate;
    unsigned used : 1;
} ckit_memory_block;

typedef struct {
    ckit_memory_block *allocations;
    int count;
    int total;
    int live;
    long bytes;
} ckit_memory_info;


static ckit_memory_info infos = {NULL, 0, 0, 0, 0L};

const ckit_memory_info *memory_infos(void){
    return &infos;
}

static ckit_memory_block *find_in_list(void *ptr){
    ckit_memory_block *available_alloc = NULL;
    int i;
    for(i = 0; i < infos.count; i++){
        if(infos.allocations[i].ptr == ptr){
            return &infos.allocations[i];
        } else if(!infos.allocations[i].used) {
            available_alloc = &infos.allocations[i];
        }
    }
    // We create a new one...
    if(available_alloc == NULL){
        infos.allocations = realloc(infos.allocations, sizeof(ckit_memory_block) * (infos.count+1));
        available_alloc = &infos.allocations[infos.count++];
    }
    available_alloc->used = 0;
    return available_alloc;
}

static void add_to_list(void *ptr, int size){
    ckit_memory_block *alloc = find_in_list(ptr);
    alloc->ptr = ptr;
    alloc->used = 1;
    alloc->size = size;
    if(alloc->allocationDate == 0){
        alloc->allocationDate = time(NULL);
    }
}

static void remove_from_list(void *ptr){
    ckit_memory_block *infos = find_in_list(ptr);
    infos->ptr = NULL;
    infos->used = 0;
    infos->size = 0;
    infos->allocationDate = 0;
}


void check_validity(uint8_t *ptr, int size) {
    // int i;
    // for(i=0; i < MAKER_SIZE; i++){
    //     if(ptr[i - MAKER_SIZE] == BEGIN_CHECK[i] || ptr[i + size] == END_CHECK[i]){
    //        ckit_exit("Overridden memory.");
    //    }
    //}
    assert(sizeof(int64_t) == MAKER_SIZE);
    int begin = memcmp(ptr - MAKER_SIZE, BEG_CHECK, MAKER_SIZE);
    int end = memcmp(ptr + size, END_CHECK, MAKER_SIZE);
    if(begin != 0 || end != 0){
        ckit_exit("Overridden memory.");
    }
}

static uint8_t *debug_alloc(int size){
    int i;
    uint8_t *ptr = malloc(size + MAKER_SIZE * 2);
    memcpy(ptr, BEG_CHECK, MAKER_SIZE);
    ptr += MAKER_SIZE;
    memcpy(ptr + size, END_CHECK, MAKER_SIZE);
    add_to_list(ptr, size);
    infos.total++;
    infos.live++;
    infos.bytes += size;
    return ptr;
}


static void debug_free(uint8_t * ptr) {
    ckit_memory_block *alloc = find_in_list(ptr);
    if(!alloc->used){
        ckit_exit("Try to free a bad pointer.");
    }
    check_validity(ptr, alloc->size);
    alloc->ptr = NULL;
    alloc->used = 0;
    alloc->size = 0;
    alloc->allocationDate = 0;
    infos.live--;
    infos.bytes -= alloc->size;
    free(ptr - MAKER_SIZE);
}


/**
 * The reallocation is quite long because, in any case,
 * the memory is allocated to receive a copy.
 */
static void *debug_realloc(uint8_t *ptr, int newsize){
    ckit_memory_block *infos = find_in_list(ptr);
    if(!infos->used){
        ckit_exit("Try to reallocate memory an used pointer.");
    }
    check_validity(infos->ptr - 8, infos->size);
    uint8_t* newptr = debug_alloc(newsize);
    memcpy(newptr, infos->ptr, infos->size);
    debug_free(infos->ptr);
    infos->size = newsize;
    infos->ptr = newptr;
    infos->used = 1; // because the block is freed.
    return newptr;
}

void *ckit_alloc(int size){
    uint8_t *ptr;
#ifdef CKIT_DEBUG
    ptr = debug_alloc(size);
#else
    ptr = malloc(size);
#endif
    if(ptr == NULL){
        ckit_exit("Unable to allocate memory.");
    }
    return ptr;
}


void ckit_free(void *ptr){
    if(ptr == NULL){
        return;
    }
#ifdef CKIT_DEBUG
    debug_free(ptr);
#else
    free(ptr);
#endif
}

void *ckit_realloc(void *ptr, int newsize){
    void *new_ptr;
    if(ptr == NULL){
        // We adhere to realloc contract.
        return ckit_alloc(newsize);
    }
#ifdef CKIT_DEBUG
    new_ptr = debug_realloc(ptr, newsize);
#else
    new_ptr = realloc(ptr, newsize);
#endif
    if(new_ptr == NULL){
        ckit_exit("Unable to reallocate memory.");
    }
    return new_ptr;
}

void *ckit_allocate_object(const char *name, int size) {
    ckit_exit("NOT IMPLEMENTED");
    return NULL;
}


void ckit_infos(FILE *f){
#ifdef NDEBUG
    fprintf(f, "RELEASE MODE\n");
#endif
#ifdef CKIT_DEBUG
    fprintf(f, "CKIT DEBUG MODE\n");
    const ckit_memory_info *infos = memory_infos();
    fprintf(f, "MEMORY:\n- blocks: %i\n", infos->count);
    fprintf(f, "- total: %i\n", infos->total);
    fprintf(f, "- live: %i\n", infos->live);
    fprintf(f, "- bytes: %li\n", infos->bytes);
    if(infos->total > 0){
        size_t nb = 0;
        size_t minsize = SIZE_MAX;
        size_t maxsize = 0;
        long totalsize = 0;
        long empty = 0;
        for(int i = 0; i < infos->count ; i++){
            if(infos->allocations[i].used){
                empty++;
            } else {
                if(minsize > infos->allocations[i].size) minsize = infos->allocations[i].size;
                if(maxsize < infos->allocations[i].size) maxsize = infos->allocations[i].size;
                totalsize += infos->allocations[i].size;
                nb++;
            }
        }
        fprintf(f, "- misize: %zu\n", minsize);
        fprintf(f, "- maxsize: %zu\n", maxsize);
    }
#endif
}