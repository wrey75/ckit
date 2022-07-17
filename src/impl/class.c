
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include "../include/memory.h"
#include "../include/class.h"


struct ckit_extra_info
{
    struct ckit_definition_class *class_ptr;
};

#define EXTRA_BYTES sizeof(struct ckit_extra_info)

struct ckit_definition_class ** ckit_classes = NULL;
static int registered_classes = 0;

void *ckit_new_object(struct ckit_definition_class *def)
{
    struct ckit_extra_info *data = ckit_alloc(def->size + EXTRA_BYTES);
    int8_t *ptr = (int8_t *)data + EXTRA_BYTES;
    assert(def != NULL);
    assert(def->construct_fnct != NULL);
    (*def->construct_fnct)(ptr);
    data->class_ptr = def;
    data->class_ptr->in_use++;
    data->class_ptr->total++;
    if(data->class_ptr->list == NULL){
        // We register the class at first allocation.
        ckit_classes = ckit_realloc(ckit_classes, sizeof(ckit_object_ptr) * (1+registered_classes));
        ckit_classes[registered_classes++] = data->class_ptr;
    }
    
    ckit_object_ptr *newptr = NULL;
    for (int i = 0; !newptr && i < def->allocated; i++)
    {
        if (def->list[i].ptr == NULL)
        {
            newptr = &def->list[i];
        }
    }
    if (newptr == NULL)
    {
        // Add an entry
        def->list = ckit_realloc(def->list, (def->allocated + 1) * sizeof(ckit_object_ptr));
        newptr = &def->list[def->allocated];
        def->allocated++;
    }
    newptr->ptr = ptr;
    newptr->used = 1;
    return ptr;
}

void *ckit_del_object(void *ptr)
{
    struct ckit_extra_info *data;
    data = (struct ckit_extra_info *)(((int8_t *)ptr) - EXTRA_BYTES);
    int found_allocation = 0;
    for (int i = 0; i < data->class_ptr->allocated; i++)
    {
        if (data->class_ptr->list[i].ptr == ptr)
        {
            // free for future use
            data->class_ptr->list[i].used = 0;
            found_allocation = 1;
        }
    }
    assert(found_allocation);
    (*data->class_ptr->finalize_fnct)(ptr);
    data->class_ptr->in_use--;
    ckit_free(data);
    return NULL;
}

void ckit_object_list(FILE *f){
    fprintf(f, "                     Class name     Live  Maximum  Allocated\n");
    fprintf(f, " ------------------------------ -------- -------- ----------\n");
    for(int i = 0; i < registered_classes; i++){
        struct ckit_definition_class *class_info = ckit_classes[i];
        fprintf(f, " %30s %8i %8i %10i\n", class_info->name, class_info->in_use, class_info->allocated, class_info->total);
    }
}
