# Heap handler

To use a min heap it is needed to first declare and initialize the handler structure.

To define a new heap handler structure the `MinHeap` macro can be used with the type of
the items and the maximum capacity given as input.

The advantages of the macro instead of a custom definition of a structure is that
the user can specify the type of the item and the maximum capacity of the buffer
from the code itself without using other macros or compiler flags.

The fields of the handler structure are:

- `data_size`: The size of a single element in **bytes**
- `size`: The current amount of elements in the heap
- `capacity`: The maximum number of items that the heap can handle (the size of the array)
- `compare`: A pointer to a function that should compare two items of the heap
- `data`: The actual array of data

!!! NOTE
    The array of data is actually one item bigger than the specified capacity because
    the last element is used as swap space during the heap operations.

The compare function should return `-1` if the value of first item has to be considered
less than the the value of the second, `0` if they are equal and `1` otherwise.

The equality check is important becuase it is also used inside the library to find
an element inside the heap.

## Handler initialization

The heap handler require to be initialize correctly before it is used, this can be done
via another convenient macro (that have to be used inline) or via the appropriate init function.

The initialization require the same type and capacity parameters used in the handler
declaration because they are used to initialize the `data_size` and `capacity` fields,
as well as the pointer to the compare function.

### Handler definition macros

The `MinHeap` macro is just a shorthand for an **anonymous struct** declaration which
contains all the data used by the heap as well as a static array of the given type and capacity.

The `min_heap_new` macro works the same as the previous but it expands to a struct
initialization instead.

To summarize, the following line of code
```c
MinHeap(int, 10) heap = min_heap_new(int, 10, int_compare_callback);
```

can be expanded as follows:

```c
struct {
    // Other fields...
    uint16_t data_size,
    int8_t (* compare)(void *, void *);
    int data[10 + 1];
} heap = {
    // Other fields...
    .data_size = sizeof(int),
    .compare = int_compare_callback,
    .data = { 0 }
};
```

!!! INFO
    This trick works because **every information can be obtained at compile time**, like
    the size of the items and the capacity of the array

## Handler interface

Since the handler structure is anonymous and it is different each time it is defined
a structure interface is needed to access all the possible handlers independently
from the type of the items and the capacity of the arrays (thus requiring the `data_size`
and `capacity` fields).

One of the problems is that the C compiler often complains and does not allow
implicit conversion of the handler structure, this can be fixed by explicitely
casting the handler to the `MinHeapInterface` type.

To avoid casting every time a function is called, basic macro wrappers are defined
for each function inside the library.

