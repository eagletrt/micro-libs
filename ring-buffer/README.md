# RING-BUFFER

A [ring buffer](https://en.wikipedia.org/wiki/Circular_buffer) (also called a *circular buffer*, or *cyclic buffer*) 
is a [FIFO](https://en.wikipedia.org/wiki/FIFO_(computing_and_electronics)) based data structure
(like a [queue](https://en.wikipedia.org/wiki/Queue_(abstract_data_type)) for example),
where the data can be pushed and pulled as if it is a data stream.

This library provide a simple enough implementation of a ring buffer whithout dynamic allocation
which is also interrupt safe

> [!NOTE]
> The buffer can be thought as an array starting from the left and ending to the right
> where the items can be pushed or popped from the *front* or the *back*, if pushed from the
> front the buffer grows to the left, otherwise if pushed to the back it grows to the right

## Usage

To create a ring buffer first declare a variable using the `RingBuffer` macro
by providing the item type and the maximum number of element of the buffer. \
For example:
```c
RingBuffer(int, 10) int_buf = ...;
RingBuffer(double, 7) double_buf = ...;
RingBuffer(struct Point, 2000) point_buf = ...;
```

Then initialize the buffer using the `ring_buffer_new` macro that requires
the same item type and capacity given in the declaration.
```c
... = ring_buffer_new(int, 10, cs_enter, cs_exit);
... = ring_buffer_new(double, 7, cs_enter, cs_exit);
... = ring_buffer_new(struct Point, 2000, cs_enter, cs_exit);
```

The following function can also be used to initialize the buffer:
```c
ring_buffer_init(&int_buf, int, 10, cs_enter, cs_exit);
ring_buffer_init(&double_buf, double, 7, cs_enter, cs_exit);
ring_buffer_init(&point_buf, struct Point, 2000, cs_enter, cs_exit);
```

The `cs_enter` and `cs_exit` function have to be implemented by the user
and should define the start and the end of a critical section respectively. \
With critical section it is intended a block of code in which an interrupt can happen
causing undefined behaviour with the data modified inside the block.

> [!NOTE]
> `NULL` can be passed in place of the `cs_enter` and `cs_exit` functions, in that case
> the ring buffer is not guaranteed to always work in case of interrupts

Here is an example of an implementation to disable, enable and recover interrupts on a
STM32 microcontroller:
```c
uint32_t primask = 0;
void cs_enter(void) {
    primask = __get_PRIMASK();
    __disable_irq();
}
void cs_exit(void) {
    if (!primask)
        __enable_irq();
}
```

> [!WARNING]
> The example above works by disabling **ALL** the interrupts and by recovering
> the previous state afterwards, this should be used carefully

The `RingBufferReturnCode` enum is return by most of the functions of this library
and **should always be checked** before attempting other operations with the data structure.

## Examples

Here is a complete example of a circular buffer of integers:
```c
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ring-buffer.h"

uint32_t primask = 0;
void cs_enter(void) {
    primask = __get_PRIMASK();
    __disable_irq();
}
void cs_exit(void) {
    if (!primask)
        __enable_irq();
}

int main(void) {
    srand(time(NULL));
    RingBuffer(int, 10) int_buf = ring_buffer_new(int, 10, cs_enter, cs_exit);

    // Push items in the buffer
    for (int i = 0; i < 5; ++i) {
        int num = rand() % 100 + 1;
        if (ring_buffer_push_back(&int_buf, &num) != RING_BUFFER_OK)
            printf("[ERROR]: Cant push element inside the buffer\n");
    }

    // Get items and info about the buffer
    printf("Buffer size: %lu\n", ring_buffer_size(&int_buf));
    int val = 0;
    if (ring_buffer_front(&int_buf, &val) == RING_BUFFER_OK)
        printf("Front element: %d\n", val);
    int * p_val = ring_buffer_peek_back(&int_buf);
    if (p_val != NULL)
        printf("Back element: %d\n", *p_val);

    // Remove items from the buffer
    printf("Values: ");
    while(!ring_buffer_is_empty(&int_buf)) {
        if (ring_buffer_pop_back(&int_buf, &val) == RING_BUFFER_OK)
            printf("%d ", val);
    }
    printf("\n");

    // Clear the buffer
    if (ring_buffer_clear(&int_buf) != RING_BUFFER_OK)
        printf("[ERROR]: Cant clear buffer\n");

    return 0;
}
```

