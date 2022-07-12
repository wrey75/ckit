
#ifndef __CKIT_CLASS_H__
#define __CKIT_CLASS_H__

struct ckit_definition_class {
    const char *name;
    const int size;
    void (*alloc_members)(void *ptr);
    void (*dispose_members)(void *ptr);
};


#define CLASS_DECLARE(a) \
    typedef struct ckit__##a a; \
    extern struct ckit_definition_class ckit_class_##a;

#define DEFINE_CLASS(a) struct ckit__##a \

#define END_CLASS(a) \
    typedef struct ckit__##a a; \
    struct ckit_definition_class ckit_class_##a;


#define NEW(a)   ((a *)ckit_new_object(&ckit_class_##a))
#define DEL(ptr) (ptr = ckit_del_object(ptr))

extern void *ckit_new_object(struct ckit_definition_class*);
extern void *ckit_del_object(void *);


#define ALLOCATE_DEFINITION(a) \
    struct ckit_definition_class ckit_class_##a; \
    extern void ckit_allocate_##a(a *ptr)
    
#define DISPOSE_DEFINITION(a) extern void ckit_dispose_##a(a* ptr)
#define CLASS_TYPE(a) (a->ckit_class_name)


#endif