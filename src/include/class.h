
#ifndef __CKIT_CLASS_H__
#define __CKIT_CLASS_H__

typedef struct
{
    void *ptr;
    unsigned used : 1;
} ckit_object_ptr;

struct ckit_definition_class
{
    int magic;
    const char *name;  // class name
    const size_t size; // size of the object
    void (*construct_fnct)(void *ptr);
    void (*finalize_fnct)(void *ptr);
    ckit_object_ptr *list;
    int allocated;
    int in_use;
    int total;
};

#define CLASS_DECLARE(a)        \
    extern struct ckit_definition_class ckit_class_##a; \
    typedef struct ckit__##a a;

#define DEFINE_CLASS(a) struct ckit__##a
#define CKIT_MAGIC_CLASS_NUMBER  0x0bec

/**
 * @brief Register the class
 *
 * Note we need 2 static methods to avoid a compilation warning. Not
 * a good way to do due to a specific call...
 */
#define REGISTER_CLASS(a) \
    static void ckit_construct0_##a(void *ptr)      \
    {                                               \
        ckit_construct_##a((a *)ptr);               \
    }                                               \
    static void ckit_finalize0_##a(void *ptr)       \
    {                                               \
        ckit_finalize_##a((a *)ptr);                \
    }                                               \
    struct ckit_definition_class ckit_class_##a = { \
        .magic = CKIT_MAGIC_CLASS_NUMBER,           \
        .name = #a,                                 \
        .size = sizeof(struct ckit__##a),           \
        .construct_fnct = &ckit_construct0_##a,     \
        .finalize_fnct = &ckit_finalize0_##a,       \
        .list = NULL,                               \
        .allocated = 0,                             \
        .in_use = 0,                                \
        .total = 0,                                 \
    };

#define NEW(a) ((a *)ckit_new_object(&ckit_class_##a))
#define DEL(ptr) (ptr = ckit_del_object(ptr))

extern void *ckit_new_object(struct ckit_definition_class *);
extern void *ckit_del_object(void *);

#define ALLOCATE_DEFINITION(a)                   \
    struct ckit_definition_class ckit_class_##a; \
    static void ckit_construct_##a(a *self)

#define DISPOSE_DEFINITION(a) static void ckit_finalize_##a(a *self)
// #define CLASS_NAME(a) (a->ckit_class_name)

#endif