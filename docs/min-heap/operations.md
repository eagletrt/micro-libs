# Operations

The first element is the heap is usually considered at the **top** of the structure.

The main operations of the min heap are:

1. [Access](#access)
2. [Insertion](#insertion)
3. [Removal](#removal)
4. [Search](#search)

## Return codes

Most of the functions returns a code that describes if the operation was executed
succesfully or, in case of failure, what was the general cause of it.

The possible returns code are:

- `MIN_HEAP_OK`: if the operation ended as expected
- `MIN_HEAP_NULL_POINTER`: if one or more NULL pointers are passed as parameters
- `MIN_HEAP_EMPTY`: if an operation tries to get or remove an element when the buffer is empty
- `MIN_HEAP_FULL`: if an operation tries to add an element when the buffer is full
- `MIN_HEAP_OUT_OF_BOUNDS`: if a given index is greater or equal than the current size of the heap

## Access

Only the top element can be accessed with a time complexity of O(1).

There are two possible ways to access the data inside the heap:

1. Directly with a pointer
2. Indirectly with a copy of the object

Direct access is faster because it does not require a copy of the object but does
not guarantee that the data does not change, and it shuold be used wisely.

The **top** function reutrns a copy of the item while the **peek** function returns
a pointer to the first element allowing direct access.

The access operations does not modify the content of the heap.

## Insertion

The worst time complexity of an insertion operation is O(log N).

If the heap is full no items are inserted and the function returns immediately.

When a new item is inserted it is placed at the end of the heap (at the index `size`),
then it is compared with the other items and eventually swapped until the *minimum
heap proprerties* are restored.

## Removal

The remove operation require the **index** of the element to obtain the worst time
complexity of O(log N), otherwise the item has to be found increasing the complexity to O(N).

Removal of **any** item in the heap is supported.

If the heap is empty no items are removed and the functions immediately.

When an item is removed it is swapped with the last element and then they are compared
to choose in which direction the restore algorithm should be applied:

- If the removed element is equal to the last element nothing has to be done
- If the removed element is less than the last one the item is *heapified up*,
 that's because, in this case, the subtree of the item is guaranteed to be heapified
 but the parents values can be greater than the last element value
- If the removed element is greater than the last one the item is *heapified down*,
 that's because, in this case, the parents of the item are guaranteed to be heapified
 but the childrens values can be lower than the last element value

## Search

Since the heap is a **partially ordered** data structure a time complexity of O(log N)
is required to find an element.

If the heap is empty the function returns immediately with a negative response.

## Other

There are also other possible operations listed below:

- Check if the heap is empty
- Check if the heap is full
- Get the current size of the heap
- Clear the heap

The **clear** operation from the list above is the only one that modifies the
data structure.

Only the `size` field is modified by the clear function and the old data is not zeroed.

