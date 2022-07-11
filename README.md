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


