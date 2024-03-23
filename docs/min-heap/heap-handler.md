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

