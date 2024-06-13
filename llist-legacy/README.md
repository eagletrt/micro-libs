# LLIST

Linked list implementation in C

## Usage

`llist.h` exposes two structures: `llist` and `llist_node`: The former defines a linked list, while the latter references a linked list node.

### Comparation functions

When initializing a linked list with `llist_init`, two optional function can be provided: a comparator function and an equals function. Their type is defined by the `comparator` and `equal` type.

A typical initialization looks like this:
```
llist test_list = llist_init(test_compare, test_equals);
```

The comparator should take two `llist_node`s (`a` and `b`) and return `1` if `a < b`, `0` if `a == b` and `-1` otherwise.

The equals function should return 1 if the provided nodes are the same, 0 otherwise.

### Return values

all the functions of this library have a standardized return value, described by the `LLIST_RETURN` enum.
The possible return values are:

- `LLIST_SUCCESS`: All good
- `LLIST_NODE_NOT_FOUND`: The node was not found
- `LLIST_EQUALS_MISSING`: The equals function is required but was not provided
- `LLIST_COMPARATOR_MISSING`: The comparator function is required but was not provided
- `LLIST_NULL_ARGUMENT`: A given argument has NULL value
- `LLIST_MALLOC_ERROR`: Error in allocating resources
- `LLIST_ERROR`: Generic error

For documentation on other functions, refer to `llist.h` which contains specific documentation for all functions.
