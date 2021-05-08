# priority-queue

This priority queue is specifically tuned to work on CAN-bus networks to deliver messages based on their priority and the time they have been waiting in queue. In order to have the most predictable impact on the MCU's memory usage, all nodes are pre-allocated at the time of initialization.

### Priority and Starvation

A message's priority corresponds to its CAN ID at the moment of insertion: the lower the ID, the higher its priority. In order to avoid starvation effects should the bus be heavily loaded with low priority messages, each pull operation decrements every message's priority by 1, so that even those entries with very high IDs eventually reach the head of the queue.

### Memory Safety

This library presents no evident leaks. Running 500 iterations of the test suite and analyzing the exacutable with valgrind show no memory defects:

```
$ valgrind --tool=memcheck --leak-check=full --show-reachable=yes test
==497486== Memcheck, a memory error detector
==497486== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==497486== Using Valgrind-3.16.1 and LibVEX; rerun with -h for copyright info
==497486== Command: test
==497486==
==497486==
==497486== HEAP SUMMARY:
==497486==     in use at exit: 0 bytes in 0 blocks
==497486==   total heap usage: 30 allocs, 30 frees, 4,065 bytes allocated
==497486==
==497486== All heap blocks were freed -- no leaks are possible
==497486==
==497486== For lists of detected and suppressed errors, rerun with: -s
==497486== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

### Instructions

##### Inclusion

The suggested way of including this library in your project is to add it as a git submodule:

```shell
$ git submodule add https://github.com/eagletrt/micro-libs
```

This way, you will be able to easily update the code within git itself, should any new commit be pushed in the future.

##### Integration

All exposed functions expect the user to be adopting a user-defined, fixed-size type as payload. Its size must be provided during initalization to properly allocate all the necessary memory.

To avoid conflicts with the existing user code, all module public methods and types are prefixed with `PQ_`.

##### Usage

To create a new queue, instatiate a new variable of type `PQ_QueueTypeDef` and initialize it with `PQ_init(...)`:

```c
typedef struct payload {
    // ...
} payload;

// ...

PQ_QueueTypeDef q;
PQ_init(&q, sizeof(payload));
```

You can check if the queue is empty with:

```c
PQ_is_empty(q);
```

To insert new elements, use `PQ_insert(...)`. Note that the passed CAN message is copied and never modified by the library.

```c
PQ_insert(q, priority, &payload);
```

Retrieving elements is divided in two operations: first, you retrieve a reference to the highest priority element with `PQ_peek_highest(...)`, and after using the data you pop and free the node with `PQ_pop_highest(...)`:

```c
payload * p;
p = PQ_peek_highest(q);
// ...
PQ_pop_highest(q);
```

After using the structure, invoke proper disposal to avoid memory leaks with:

```c
PQ_destroy(&q);
```

### Documentation

All code is document with Doxygen-style comments

### Testing

To run code tests locally:

- `cd` into `test/`
- Pull the `munit` git submodule
- Run `make test && ./test`
