
#include <stdio.h>
#include <stdlib.h>
#include "../ckit.h"

/**
 * Initialize the kit.
 * 
 */
void ckit_init(void) {

}

void ckit_exit(const char *message){
    fprintf(stderr, "**FATAL**: %s", message);
    exit(1);
}


void *ckit_new_object(struct ckit_definition_class *def){
    return ckit_alloc(def->size);
}

void *ckit_del_object(void *ptr){
    ckit_free(ptr);
    return NULL;
}

