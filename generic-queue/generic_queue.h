#ifndef _GENERIC_QUEUE
#define _GENERIC_QUEUE

#include "inttypes.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

typedef struct{
    size_t head, tail;
    size_t size;
    size_t queue_elem_size; 
    size_t max_elems;
    int cnt_elems;
    uint8_t *queue;
} generic_queue_t;

/**
 * 
 * @param q pointer to the queue
 * @param size size of the allocated memory in bytes
 * @param elem_size max size of the single element of the queue
 * @param queue pointer to the allocated memory for the queue
*/
void GENQ_init(generic_queue_t *q, size_t size, size_t elem_size, uint8_t *queue);
bool GENQ_is_empty(generic_queue_t *q);
bool GENQ_is_full(generic_queue_t *q);
bool GENQ_pop(generic_queue_t *q, uint8_t *e);
bool GENQ_push(generic_queue_t*, uint8_t *e);

#endif