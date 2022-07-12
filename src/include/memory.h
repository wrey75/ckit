
#ifndef MEMORY_H__
#define MEMORY_H__

extern void *ckit_alloc(int size);
extern void ckit_free(void *);
extern void *ckit_realloc(void *, int newsize);


#endif
