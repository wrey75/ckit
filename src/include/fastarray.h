/**
 * FASTARRAY is a very basic way to allocate arrays. As for the allocation of
 * memory, you can allocate, reallocate and free arrays. An array is made of
 * elements of a spcified size. Then you must allocate first with the size of
 * an element and the size of the array. Then reallocation is made on the size
 * of the array.
 * 
 * Note the number of elements can be found by calling ckit_array_size(). The 
 * reallocation is a real reallocation and use strictly the number of elements
 * given as parameter.
 * 
 * These function are like calloc() in the sense the new allocated memory is 
 * cleaned up with zeros.
 *  
 */
#ifndef CKIT__FASTARRAY_H
#define CKIT__FASTARRAY_H

#define ARRAY_ALLOC(s,t) (t *)ckit_array_alloc(s, sizeof(t))

/**
 * @brief allocates an array of `arry_size` elements which every elements
 * having a size of `element_size` bytes. Similar to "calloc()".
 * 
 * @param count the number of elements. Can be zero. 
 * @param size the size of one element.
 * @return void* the pointer to the array.
 */
extern void *ckit_array_alloc(size_t count, size_t size);

/**
 * @brief re-allocates a previously allocated array.
 * 
 * @param ptr the original pointer. 
 * @param count the new number of elements (can shrink the size or add new entries).
 * @return void* a new pointer to the newly allocated memory.
 */
extern void *ckit_array_realloc(void *ptr, size_t count);

/**
 * @brief frees a previously allocated array. The pointer must be first allocated
 * by "ckit_array_alloc()".
 * 
 * @param ptr the previously allocated pointer.
 */
extern void ckit_array_free(void *ptr);

/**
 * @brief The number of elements of the array.
 * 
 * @param ptr the pointer previously retrieved from "ckit_array_alloc()".
 * @return size_t the number of entries available in the array.
 */
extern size_t ckit_array_size(void *ptr);

/**
 * @brief This function is for internal use only.
 */
extern void *ckit_magic_alloc(size_t size, const char *magic);

/**
 * @brief This function is for internal use only.
 */
extern void ckit_magic_free(void* ptr, const char *magic);

#endif