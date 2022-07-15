#ifndef CKIT_HASHTABLE_H__
#define CKIT_HASHTABLE_H__

CLASS_DECLARE(Hashtable)

void hash_clear(Hashtable *self);
void hash_resize(Hashtable *self, int newsize);
void hash_size(const Hashtable *self);
void hash_dump(const Hashtable *self, FILE *f);
const void * hash_set(Hashtable *self, const char *key, const void *value);
const void * hash_get(const Hashtable *self, const char *key);

#endif
