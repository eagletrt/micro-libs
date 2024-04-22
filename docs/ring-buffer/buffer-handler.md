# Buffer handler

To use a ring buffer it is needed to first declare and initialize the handler structure.

To define a new ring buffer handler structure the `RingBuffer` macro can be used with
the type of the items and the maximum capacity given as inputs.

The advantages of the macro instead of a custom definition of a structure is that the user
can specify the type of the item and the maximum capacity of the buffer from the code itself
without using other macros or compiler flags

The fields of the hanlder structure are:

- `start`: The index of the first element in the buffer
- `size`: The current amount of elements in the buffer
- `data_size`: The size of a single item in **bytes**
- `capacity`: The maximum number of items that the buffer can handle (the size of the array)
- `cs_enter`: A pointer to a function that should manage a [critical section](critical-section.md) (can be NULL)
- `cs_exit`: A pointer to a function that should exit a [critical section](critical-section.md) (can be NULL)
- `data`: The actual array of data

## Handler initialization

The buffer handler require to be initialized correctly before it is used, this can be done
via another convenient macro (that have to be used inline) or via the appropriate init function.

The initialization require the same type and capacity parameters used in the handler declaration
because they are used to initialize the `data_size` and `capacity` fields, as well as the pointers
to the critical section handler functions.

If the critical section handler functions are `NULL` an empty *dummy* function
defined inside the library is used instead to avoid null checks.

### Handler definition macros

The `RingBuffer` macro is just a shorthand for an **anonymous struct** declaration which
contains all the data used by the buffer as well as a static array of the given type and capacity.

The `ring_buffer_new` macro works the same as the previous but it expands to a struct
initialization instead.

To summarize, the following line of code
```c
RingBuffer(int, 10) buf = rin_buffer_new(int, 10, NULL, NULL);
```

can be expanded as follows:

```c
struct {
    // Other fields...
    uint16_t data_size;
    void (*cs_enter)(void);
    void (*cs_exit)(void);
    int data[10];
} buf = {
    // Other fields...
    .data_size = sizeof(int),
    .cs_enter = NULL != NULL ? NULL : _ring_buffer_cs_dummy,
    .cs_exit = NULL != NULL ? NULL : _ring_buffer_cs_dummy,
    .data = { 0 }
};
```

!!! INFO
    This trick works because **every information can be obtained at compile time**, like
    the size of the items and the capacity of the array

## Handler interface

Since the handler structure is anonymous and it is different each time it is defined
a structure interface is needed to access all the possible handlers independently from
the type of the items and the capacity of the array (thus requiring the `data_size` and `capacity` fields).

One of the problems is that the C compiler often complains and does not allow implicit
conversion of the handler structure, this can be fixed by explicitely casting the handler
to the `RingBufferInterface` type.

To avoid casting every time a function is called, basic macro wrappers are defined
for each function inside the library.

