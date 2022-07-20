# ckit
A development kit for C developments

## Protecting memory

The kit provides 3 replacement for memory allocation:

- `ckit_alloc` (replaces `malloc`) to allocate memory.
- `ckit_free` (replaces `free`) to free the memory allocated by `ckit_alloc`^.
- `ckit_realloc` (replaces `realloc`) to reallocate memory (basically extends the size).

The `DEBUG` versions are very conservative and slows down the programs but can ensure 
no memory is overridden. As the `NDEBUG` version are simply calls to their respective
counterparts to the CLib library.

## Classes

The system offers classes like in more advanced languages. Of course, this is a mimic
of object-oriented languages like JAVA. But provides quite simple capabilities.

### Declare a class

A class is declared through the `CLASS_DECLARE` macro. You simply add the simple line
in your header (`.h`) file:

    CLASS_DECLARE(UString)

Then it provides a hidden class when the properties are not visible. Yoou just have to
add the C functions like this:

    extern void ustr_concat(UString *, const char *);
    extern void ustr_add(UString *, const UString *);

In the `.c` part, you just provide the declaration of the class and the functions.

    DEFINE_CLASS(UString) {
        wchar_t *buf;
        uint16_t size;
    };

is the definition of the `UString` object. Basically, an array and the size of it. It is a
very basic class. In the C part of the code, you can access the fields as usual with a `struct`.

Note you MUST define 3 functions:

    ALLOCATE_DEFINITION(UString)
    {
        self->size = 1;
        self->buf = ckit_alloc(sizeof(wchar_t));
        self->buf[0] = 0;
    }

    DISPOSE_DEFINITION(UString) {
        ckit_free(self->buf);
    }

    REGISTER_CLASS(UString)

The first one is the constructor to initialize the contents of the class and the second one
is the finalizer to clean up the fields. Even these function can be empty, you must decalre them.
"self" is the pointer to the allocated object (note the allocation on the object is made behind the scene).

The last one is not a function but this macro is needed to declare the stuff related to the class. Note
this information is mainly to maintain the information related to the class.

### Allocates an instance

To create an object, use the `NEW` macro (same as in C++) and the `DEL` macro to delete it.

    UString *str = NEW(UString);

declare a new string but works for any declared object.

## Performances

Note the performance is very poor in DEBUG mode due to the numerous extra code running to maintain
everything fine. In addtion, reallocation is not optimzed at all. But the "NDEBUG" mode will just
performs very fast.

Some optimization will be done on objects to avoid the allocation and destruction. Using zombies will
add big performances to your program.

## Basic allocations

Memory allocation is made through 3 specific function that mimics the original ones in C ANSI:

- `ckit_alloc()` mimics the `malloc()` function and returns a pointer of the specified requested size.
NOTE the allocated memory is NOT initialized as for `malloc()` function.
- `ckit_realloc()` mimics the `realloc()` function and allocates or shrink the original memory. Note
the returned pointer _could_ but is _not necessary_ the same pointer than the original one. In DEBUG
mode, the pointer is always a new one. This is a more secure way to achieve development.
- `ckit_free()` mimics `free()` function and frees the memory allocacated first by `ckit_alloc()`. Note
you can safely call this method with a NULL pointer value.

Those 3 functions mimics the original ones. In RELEASE mode (`NDEBUG` is set), there are really the
C original functions. You can reallocate not initialized memory if you provide a NULL pointer to
`ckit_realloc`.

In DEBUG mode, there are some controls about the "end" and the "beginning" of the data and the memory
really allocated is 16-byte longer to store this information. When the memory is reallocated or freed
in DEBUG mode, there is a control of the validity of the memory space and the program is exited if the
memory has been ovverridden.


## Fast arrays

We have 3 other allocations for arrays mainly.

`void *ckit_array_alloc(size_t count, size_t size)` is to allocate an array of `count`
elements each of them having a size of `size` bytes. Note the allocated memory is set to zero. 
You don't have to initialize it. The `count` can be zero to initialize an empty array. Arguments are
in the same order than `calloc`.

`void *ckit_array_realloc(void *array, size_t newsize)` is to reallocate an array previously
allocated by `ckit_array_alloc`. You CAN NOT use this method to first allocate an array: you must first
initialize the array. You can extend or shrink the array depending of your needs.

`void ckit_array_free(void * array)` frees the array. Note the functions does not returns a NULL
pointer (`ckit_free()` does it). Only pointers allocated by the `ckit_array_alloc()` functions are
permitted.

`size_t ckit_array_size()` returns the count of the array (the number of elements). Thanks to a thin layer managed by the ckit
library.

NOTE: these arrays are not _fast_ in terms of performances but very useful to manipulate arrays as
pointers. We have also an `Array` object provided by the library with more capabilities and the
capacity to manage its memory more effeciently. But these functions are quite fast when used
correctly. They are used inside the library itself and provided to the user for low-level coding.






