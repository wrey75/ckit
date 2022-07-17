#ifndef CKIT_HASHTABLE_H__
#define CKIT_HASHTABLE_H__

CLASS_DECLARE(Hashtable)

extern void hash_clear(Hashtable *self);
extern void hash_resize(Hashtable *self, int newsize);
extern int hash_size(const Hashtable *self);
extern void hash_dump(const Hashtable *self, FILE *f);
extern const void * hash_set(Hashtable *self, const char *key, const void *value);
extern const void * hash_get(const Hashtable *self, const char *key);

#endif
