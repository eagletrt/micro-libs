# MIN-HEAP

This library implements all the functions used to create and manage a [minimum heap](https://it.wikipedia.org/wiki/Heap_(struttura_dati))
data structure using a static array without dynamic memory allocation.

> [!IMPORTANT]
> This library does not guarantee that the functions will be executed correctly
> if used with interrupts or multi-threaded architectures

A min heap is a **partially ordered** binary tree-based data structure that guarantees that:
1. The first element is always the minimum of all the elements
2. Each element has a value always lower than the value of the children

> [!TIP]
> A *zero-indexed* array can be represented as a tree by choosing the first element as the root node
> and the element at index $i * 2 + 1$ as the left child and $i * 2 + 2$ as the right child

This library implements:
- Insertion of an item in $O(log N)$ time complexity
- Removal of any item (given the index in the heap) in $O(log N)$ time complexity
- Search of any item in $O(N)$ time complexity

> [!NOTE]
> Removal of an item without the index requires a linear search of the array,
> which implies linear time complexity (i.e. *no bueno*)


## Usage

To create a min heap first declare a variable using the `MinHeap` macro
by providing the item type and the maximum number of element of the heap. \
For example:
```c
MinHeap(int, 10) int_heap = ...;
MinHeap(double, 7) double_heap = ...;
MinHeap(struct Point, 2000) point_heap = ...;
```

Then initialize the heap using the `min_heap_new` macro that requires the same item type
and capacity given in the declaration as well as a pointer to a function that should compare
two items of the heap.
```c
... = min_heap_new(int, 10, int_compare);
... = min_heap_new(double, 7, double_compare);
... = min_heap_new(struct Point, 2000, point_compare);
```

The `min_heap_init` function can also be used to initialize the heap structure as follows:
```c
min_heap_init(&int_heap, int, 10, int_compare);
min_heap_init(&double_heap, double, 7, double_compare);
min_heap_init(&point_heap, struct Point, 2000, point_compare);
```

The compare function signature is as follows:
```c
int8_t compare(void * a, void * b);
```

The two parameters `a` and `b` are the two items that should be compared and has to
be casted to the type of the item used inside the heap. \
The return value should be:
- Negative(-1): if `a` is *less than* `b`
- Zero(0): if `a` *equals* `b`
- Positive(1): if `a` is *greater than* `b`

For example:
```c
int8_t int_compare(void * a, void * b) {
    int f = *(int *)a;
    int s = *(int *)b;
    if (f < s) return -1;
    return f == s ? 0 : 1;
}
```

The `MinHeapReturnCode` enum is return by most of the functions of this library
and **should always be checked** before attempting other operations with the data structure.

## Examples

Here is a complete example of a minimum heap of integers:
```c
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "min-heap.h"

int8_t min_heap_compare_int(void * a, void * b) {
    int f = *(int *)a;
    int s = *(int *)b;
    if (f < s) return -1;
    return f == s ? 0 : 1;
}

int main(void) {
    srand(time(NULL));
    MinHeap(int, 10) int_heap = min_heap_new(int, 10, min_heap_compare_int);
    
    for (int i = 0; i < 5; ++i) {
        int num = rand() % 100 + 1;
        if (min_heap_insert(&int_heap, &num) != MIN_HEAP_OK)
            printf("[ERROR]: Cannot insert element in the heap\n");
    }
    printf("Heap size: %lu\n", min_heap_size(&int_heap));
    int min = 0;
    if (min_heap_top(&int_heap, &min) == MIN_HEAP_OK)
        printf("Minimum element: %d\n", min);

    printf("Values: ");
    while(!min_heap_is_empty(&int_heap)) {
        int num;
        if (min_heap_remove(&int_heap, 0, &num) == MIN_HEAP_OK)
            printf("%d ", num);
    }
    printf("\n");

    // Clear the heap
    if (min_heap_clear(&int_heap) != MIN_HEAP_OK)
        printf("[ERROR]: Cannot clear the heap\n");
    
    return 0;
}
```

