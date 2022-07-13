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




