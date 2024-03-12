# Min heap

A [minimum heap](https://en.wikipedia.org/wiki/Heap_(data_structure)) is a **tree-based** data structure
that satisfy the *min heap property*:
1. The root element is always the minimum of all the items in the heap
2. For each item in the heap the parent's value is always greater than the item's value

The *min-heap* library implements the minimum heap data structure using a fixed size array,
whitout dynamic memory allocation.

## Handler definition and initialization

The main problems of a generic data structure implementation is that the user can't
decide the **type** of the elements and/or the **maximum number of items** of the structure
without using dynamic memory allocation and void pointers.

To "solve" this issue a macro is used to define a custom item type and a custom capacity
of the array where the data is actually stored and another macro is used to initialize the min-heap. \
The macro signatures are as follows:
- `MinHeap(TYPE, CAPACITY)`
- `min_heap_init(TYPE, CAPACITY, CMP_CALLBACK)`

The `TYPE` parameter can be any type: for example integers, floats, pointers structures or even arrays. \
The `CAPACITY` is the maximum number of items that the min-heap can handle, so the total number
of bytes of the data is `sizeof(TYPE) * CAPACITY`. \
The `CMP_CALLBACK` is a pointer to a function that should compare two items and is used internally
by the min-heap library to sort and swap the elements with a particular order. \
The callback accept two *void pointers* which are the first and second element to be compared
and should return an `int8_t` which is:
- `-1` if the first item is to be considered less than the second
- `0` if the items are equal
- `1` in all the other cases

> [!NOTE]
> Any other value that can be represented with an eight bit integer can be returned from the
> callback, but it should be kept in mind that any negative value is considered to be as `-1` and
> any positive value is considered to be as `1`

When a new min-heap is defined and initialized the macro is expanded in structure definition
and initialization, so each time a new variable is declared an *anonymous* struct gets created.

The structure contains:
- The size of a single element in bytes (`sizeof(TYPE)`)
- The current size of the min-heap (starts from 0)
- The capacity of the min-heap (the maximum number of items it can hold)
- A pointer to the compare function callback
- The array of data where the items are stored

> [!NOTE]
> The data array has `CAPACITY + 1` length because the last element is used internally
> by the library to swap variables

## Handler interface

The `MinHeapInterface` structure is used to access the element inside the handler
within the library, because each defined handler is an anonymous struct with a custom
item type and array length. \
The only difference between the defined structure and the interface is the data field
which is treated as a void pointer (`int data[10]` becomes `void * data`).

All the functions in the library requires a pointer to an interface as parameter,
this require an explicit cast from the anonymous struct pointer to the interface pointer. \
To avoid casting every time a function is called a macro is defined for each function in the library
that simply calls the function with the correct cast.



