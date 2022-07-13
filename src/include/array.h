
#ifndef _CKIT_ARRAY_H__
#define _CKIT_ARRAY_H__


CLASS_DECLARE(Array)


/**
 * @brief set the callback functions
 * 
 * @param self the array
 * @param insert the insert callback or NULL
 * @param delete the delete callback or NULL
 */
extern void array_set_callbacks(Array *self, void (*insert)(void*), void (*delete)(void*));

extern void array_add(Array *self, void *ptr);
extern void array_clear(Array *self);
extern void array_remove(Array *self, int index);
extern int array_size(Array *self);
extern void *array_get(Array *self, int index);

/**
 * @brief set the specified element with the value specified.
 * 
 * NOTE: no callback function are applied on this method.
 * 
 * @param self the array
 * @param index the index to change
 * @param value the value to set
 * @return void* the old value
 */
extern void *array_set(Array *self, int index, void *value);

#endif