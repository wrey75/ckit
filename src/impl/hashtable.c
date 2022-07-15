
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../ckit.h"

#define INITIAL_SIZE 8
 
struct DataItem {
    char *key;
    const void *value;
};
 

DEFINE_CLASS(Hashtable) {
    struct DataItem** hashArray;
    size_t size;
};


ALLOCATE_DEFINITION(Hashtable)
{
    self->size = INITIAL_SIZE;
    self->hashArray = ckit_calloc(sizeof(struct DataItem), self->size);
}

DISPOSE_DEFINITION(Hashtable) {
    hash_clear(self);
    ckit_free(self->hashArray);
}

REGISTER_CLASS(Hashtable)
 
static unsigned hashCode(const char *key)
{
    unsigned hash = 0;
    if(key != NULL) {
        while(*key != 0) {
            hash = hash * 31 + (*key++);
        }
    }
    return hash;
}
 
static struct DataItem *search(const Hashtable*self, const char* key)
{
    //get the hash
    unsigned idx = hashCode(key) % self->size;
    
    struct DataItem *list = self->hashArray[idx];
    fprintf(stderr, "LIST %p - %i - %p", self->hashArray, idx, self->hashArray[idx]);
    if(list != NULL) {
        int i = 0;
        while(list[i].key != NULL) {
            if(strcmp(key, list[i].key) == 0) {
                return &list[i];
            }
 
            //go to next cell
            i++;
        }
    }
    return NULL;
}

const void *hash_get(const Hashtable*self, const char* key){
    struct DataItem *item = search(self, key);
    if(item != NULL) {
        return item->value;
    }
    return NULL;
}

static struct DataItem *new_item(Hashtable *self, const char* key)
{
    int hash = hashCode(key) % self->size;
 
    struct DataItem *list = self->hashArray[hash];
    if(list == NULL) {
        list = malloc(sizeof(struct DataItem *) * 2);
        list[0].key = NULL;
        list[1].key = NULL;
        self->hashArray[hash] = list;
        return &list[0];
    }
    // fprintf(stderr,"EXTEND...");
    int i = 0;
    // fprintf(stderr, "on %i: ", hash);
    while(list[i].key != NULL) {
        // fprintf(stderr, "(%s) ",list[i].key);
        i++;
    }
    self->hashArray[hash] = realloc(list, sizeof(struct DataItem) * (i+2));
    self->hashArray[hash][i+1].key = NULL;
    // fprintf(stderr, "now list is %i long\n", i + 1);
    return &self->hashArray[hash][i];
}
 
 
const void * hash_set(Hashtable *self, const char* key, const void * value)
{
    struct DataItem *item = search(self, key);
    fprintf(stderr, "HER3");
    if(item != NULL) {
        // we simply replace the entry
        const void * old = item->value;
        item->value = value;
        return old;
    }
 
    // We add the entry
    item = new_item(self, key);
    // fprintf(stderr, "ADDING %s [%i] - %p\n", key, value, item);
    int len = strlen(key)+1;
    char *ptr = ckit_alloc(len);
    memcpy(ptr, key, len);
    item->key = ptr;
    item->value = value;
    return 0;
}
 
int hash_delete(Hashtable *self, const char* key)
{
    struct DataItem *item = search(self, key);
    if(item == NULL) {
        return 0;
    }
    ckit_free(item->key);
    do {
        struct DataItem *next = item + 1;
       item->key = next->key;
        item->value = next->value;
        item = next;
    } while(item->key != NULL);
    return 1;
}

/**
 */
void hash_clear(Hashtable *self){
    struct DataItem **array0 = self->hashArray;
    int i;
    for(i = 0; i<self->size; i++) {
        struct DataItem *list = array0[i];
        if(list != NULL) {
            while(list->key != NULL) {
                ckit_free(list->key);
                list++;
            }
            ckit_free(array0[i]); // and free the list
        }
    }
    memset(array0, 0, sizeof(struct DataItem) * self->size);
}

void hash_resize(Hashtable *self, int newsize){
    struct DataItem **array0 = self->hashArray;
    int size0 = self->size;
    self->size = newsize;
    self->hashArray = ckit_calloc(sizeof(struct DataItem), newsize);
    int i;
    for(i = 0; i<size0; i++) {
        struct DataItem *list = array0[i];
           fprintf(stderr, "WE ARE AT %i ptr %p\n", i, list);
 
        if(list != NULL) {
            while(list->key != NULL) {
                struct DataItem *item = new_item(self, list->key);
                // fprintf(stderr, "%s=%p\n", list->key, item);
                item->key = list->key; // just move the pointer
                item->value = list->value;
                list++;
            }
            ckit_free(array0[i]); // and free the list
        }
    }
    fprintf(stderr, "WE ARE HERE$");
    ckit_free(array0); // do not forget the hash array
}
 
void hash_dump(const Hashtable *self, FILE *f)
{
    int i = 0;
    fprintf(f, "----------\n");
    for(i = 0; i<self->size; i++) {
        struct DataItem *list = self->hashArray[i];
        fprintf(f, "%3i : ", i);
        if(list != NULL) {
            while(list->key != NULL) {
                fprintf(f, " (%s: %p); ",list->key,list->value);
                list++;
            }
        } else {
            fprintf(f, " ~ empty");
        }
        fprintf(f, "\n");
    }
    fprintf(f,"----------\n");
}
 
