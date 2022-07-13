
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "../ckit.h"

static void nothing(void *ptr)
{
}

DEFINE_CLASS(Array)
{
    void **array;
    size_t element_size;
    void (*remove_callback)(void *);
    void (*insert_callback)(void *);
    int count;
    int max;
};

ALLOCATE_DEFINITION(Array)
{
     fprintf(stderr, "HERE 0");
    self->count = 0;
    self->max = 0;
    self->array = NULL;
    self->remove_callback = &nothing;
    self->insert_callback = &nothing;
    self->element_size = sizeof(void *);
    fprintf(stderr, "HERE 1");
}

DISPOSE_DEFINITION(Array)
{
    ckit_free(self->array);
}

REGISTER_CLASS(Array)

inline static int max(a, b) { return a < b ? b : a; }

inline static void array_ensure(Array *self, int elements)
{
    fprintf(stderr, "HERE");
    if (self->max < elements)
    {
        int nb = max(10, max(elements, self->count < 2000 ? self->count * 2 : self->count + 1000));
        self->max = nb;
        self->array = ckit_realloc(self->array, self->max * self->element_size);
    }
}

extern void array_add(Array *self, void *ptr)
{
    array_ensure(self, self->count + 1);
    self->array[self->count++] = ptr;
    self->insert_callback(ptr);
}

void array_remove(Array *self, int pos)
{
    if (pos < 0 || pos >= self->count)
    {
        ckit_exit("Your index is out of bounds.");
    }
    self->remove_callback(self->array[pos]);
    memmove(&self->array[pos], &self->array[pos + 1], (self->count - pos) * sizeof(void *));
    self->array[--self->count] = NULL;
}

void array_clear(Array *self)
{
    for (int i = 0; i < self->count; i++)
    {
        self->remove_callback(self->array[i]);
        self->array[i] = NULL;
    }
    self->count = 0;
}

int array_size(Array *self)
{
    return self->count;
}

void *array_get(Array *self, int index)
{
    if (index < 0 || index >= self->count)
    {
        ckit_exit("Your index is out of bounds.");
    }
    return self->array[index];
}
void *array_set(Array *self, int index, void *value)
{
    if (index < 0 || index >= self->count)
    {
        ckit_exit("Your index is out of bounds.");
    }
    void *old = self->array[index];
    self->array[index] = value;
    return old;
}


void array_set_callbacks(Array *self, void (*insert)(void*), void (*delete)(void*)){
    assert(self->count == 0);
    self->insert_callback = (*insert == NULL ? &nothing : insert);
    self->remove_callback = (*delete == NULL ? &nothing : delete);
}
