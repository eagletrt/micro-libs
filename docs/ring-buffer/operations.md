# Operations

When an operation is done at the **front** of the buffer it means that it is done
at the start of the buffer (where the `start` field points to) otherwise if an
operation is done at the **back** it means that it is done at the end of the buffer
(where the index `start + size` points to).

The main operations of the ring buffer are:

1. [Access](#access)
2. [Insertion](#insertion)
3. [Removal](#removal)

## Return code

Most of the functions returns a code that describes if the operation was executed
succesfully or, in case of failure, what was the general cause of it.

The possible returns code are:

- `RING_BUFFER_OK`: if the operation ended as expected
- `RING_BUFFER_NULL_POINTER`: if one or more NULL pointers are passed as parameters
- `RING_BUFFER_EMPTY`: if an operation tries to get or remove an element when the buffer is empty
- `RING_BUFFER_FULL`: if an operation tries to add an element when the buffer is full

## Access

There are two possible ways to access the data inside the buffer:

1. Directly with a pointer
2. Indirectly with a copy of the object

Direct access is faster because it does not require a copy of the object but does not
guarantee that the data does not change, and it should be used wisely.

The **front** and **back** functions returns a copy of the item but if they are prefixed
with the word **peek** they return the pointer allowing direct access.

The `cs_enter` and `cs_exit` are called at the start and the end of the function respectively.

The access operation does not modify the content of the buffer.

## Insertion

The insertion operation inside a ring buffer is often called *push* and it is possible to:

- Push the element at the front of the buffer
- Push the element at the back of the buffer

The `cs_enter` and `cs_exit` are called at the start and the end of the function respectively.

If the buffer is full no items are pushed and the function return immediately.

If the element is pushed to the front the `size` as well as the `start` fields are modified,
otherwise if it is pushed to the back only the `size` is incremented.

## Removal

The removal operation inside a ring buffer is often called *pop* and it is possible to:

- Pop an element at the front of the buffer
- Pop an element at the back of the buffer

The `cs_enter` and `cs_exit` are called at the start and the end of the function respectively.

If the buffer is empty no items are poped and the function return immediately.

If the element is poped from the front the `size` as well as the `start` fields are modified,
otherwise if it is poped from the back only the `size` is decremented.

## Other

There also other possible operations listed below:

- Check if the buffer is empty
- Check if the buffer is full
- Get the current size of the buffer
- Clear the buffer

The **clear** operation from the list above is the only one that modifies the data structure
and calls the `cs_enter` and `cs_exit` functions.

Only the `start` and `size` field are modified by the clear function and the old data is
not zeroed.

