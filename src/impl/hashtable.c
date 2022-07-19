
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdbool.h>
#include <assert.h>
#include "../ckit.h"

// #define TRACE(...) fprintf(stderr, __VA_ARGS__)
#define TRACE(...)

#define INITIAL_SIZE 8

struct DataItem
{
    char *key;
    const void *value;
};

DEFINE_CLASS(Hashtable)
{
    struct DataItem **array;
    size_t count;
    size_t size;
    float factor;
};

ALLOCATE_DEFINITION(Hashtable)
{
    self->factor = 0.75;
    self->size = INITIAL_SIZE;
    self->count = 0;
    self->array = ARRAY_ALLOC(self->size, struct DataItem *);
}

DISPOSE_DEFINITION(Hashtable)
{
    hash_clear(self);
    ckit_array_free(self->array);
}

REGISTER_CLASS(Hashtable)

static unsigned hashCode(const char *key)
{
    unsigned hash = 0;
    assert(key != NULL);
    while (*key != 0)
    {
        hash = hash * 31 + (*key++);
    }
    return hash;
}

static struct DataItem *search(const Hashtable *self, const char *key)
{
    // get the hash
    unsigned idx = hashCode(key) % self->size;
    struct DataItem *list = self->array[idx];
    TRACE("Hashtable::search(): looking for '%s' at index %i (list at %p)...\n", key, idx, list);
    while (list && list->key != NULL)
    {
        // fprintf(stderr, "SCANNING...");
        if (strcmp(key, list->key) == 0)
        {
            // fprintf(stderr, "FOUND AT %p\n", list);
            return list;
        }
        list++;
        // fprintf(stderr, "NOT FOUND\n");
    }
    return NULL;
}

int hash_size(const Hashtable *self){
    return self->count;
}

const void *hash_get(const Hashtable *self, const char *key)
{
    struct DataItem *found = search(self, key);
    if (found != NULL)
    {
        return found->value;
    }
    return NULL;
}

static struct DataItem *new_item(Hashtable *self, const char *key)
{
    int hash = hashCode(key) % self->size;
    TRACE("Hastable::new_item(): looking for entry %i\n", hash);
    if (self->array[hash] == NULL){
        ckit_validate_memory();
        TRACE("Hastable::new_item(): creates a new chain for index %i at %p\n", hash, self->array[hash]); 
        self->array[hash] = ARRAY_ALLOC(2, struct DataItem);
        ckit_validate_memory();
        return self->array[hash];
    } else {
        int i = 0;
        struct DataItem* list = self->array[hash];
        assert(list != NULL);
        while(list->key != NULL){
            // fprintf(stderr, "-\n");
            i++;
            list++;
        }
        self->array[hash] = ckit_array_realloc(self->array[hash], i+2);
        TRACE("Hastable::new_item(): extending the chain for index %i to %i elements\n", hash, i);
        assert(self->array[hash][i].key == NULL);
        return &self->array[hash][i];
    }
}

const void *hash_set(Hashtable *self, const char *key, const void *value)
{
    ckit_validate_memory();
    struct DataItem *item = search(self, key);
    TRACE("hash_set(): key '%s' returns block %p \n", key, item);
    if (item != NULL)
    {
        // we simply replace the entry
        TRACE("hash_set(): replacing %s [%p] - %p\n", key, (const char *)value, item);
        const void *old = item->value;
        item->value = value;
        return old;
    }

    // We have to add the entry
    unsigned idx = hashCode(key) % self->size;
    item = new_item(self, key);
    ckit_validate_memory();
    TRACE("hash_set(): adding key '%s'at index %i (new ptr %p)\n", key, idx, item);
    int len = strlen(key) + 1;
    char *ptr = ckit_alloc(len);
    ckit_validate_memory();
    memcpy(ptr, key, len);
    item->key = ptr;
    item->value = value;
    self->count++;
    return NULL;
}

int hash_delete(Hashtable *self, const char *key)
{
    struct DataItem *item = search(self, key);
    if (item != NULL)
    {
        assert(strcmp(key, item->key) != 0);
        ckit_free(item->key);
        do {
            memcpy(item, &item[1], sizeof(struct DataItem));
            item++; 
        } while(item->key != NULL);
        self->count--;
        return 1;
    }
    return 0;
}

/**
 */
void hash_clear(Hashtable *self)
{
    struct DataItem **array0 = self->array;
    int i;
    for (i = 0; i < self->size; i++)
    {
        struct DataItem *list = array0[i];
        if (list != NULL)
        {
            while (list->key != NULL)
            {
                ckit_free(list->key);
                list++;
            }
            ckit_array_free(array0[i]); // and free the list
        }
    }
    memset(array0, 0, sizeof(struct DataItem) * self->size);
    self->count = 0;
}

void hash_resize(Hashtable *self, int newsize)
{
    struct DataItem **array0 = self->array;
    int size0 = self->size;
    self->size = newsize;
    self->array = ckit_array_alloc(newsize, sizeof(struct DataItem));
    int i;
    for (i = 0; i < size0; i++)
    {
        struct DataItem *list = array0[i];
        // fprintf(stderr, "WE ARE AT %i ptr %p\n", i, list);

        if (list != NULL)
        {
            while (list->key != NULL)
            {
                // fprintf(stderr, "%s -- ", list->key);
                struct DataItem *item = new_item(self, list->key);
                // fprintf(stderr, "%s=%p\n", list->key, item);
                item->key = list->key; // just move the pointer
                item->value = list->value;
                list++;
                
            }
            ckit_array_free(array0[i]); // and free the list
        }
    }
    TRACE("hash_resized(): has now %i entries.\n", newsize);
    ckit_array_free(array0); // do not forget the hash array
    ckit_validate_memory();
}


void hash_dump(const Hashtable *self, FILE *f)
{
    int i = 0;
    fprintf(f, "----------\n");
    for (i = 0; i < self->size; i++)
    {
        struct DataItem *list = self->array[i];
        fprintf(f, "%3i : ", i);
        if (list != NULL)
        {
            while (list->key != NULL)
            {
                fprintf(f, " (%s: %p); ", list->key, list->value);
                list++;
            }
        }
        else
        {
            fprintf(f, " ~ empty");
        }
        fprintf(f, "\n");
    }
    fprintf(f, "----------\n");
}
