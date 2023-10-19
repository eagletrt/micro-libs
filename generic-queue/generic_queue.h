#ifndef _GENERIC_QUEUE
#define _GENERIC_QUEUE

#include "inttypes.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

#define GENQ_SIZE 1000

typedef struct{
    size_t head, tail;
    uint8_t queue[GENQ_SIZE];
    size_t queue_elem_size; 
    size_t max_elems;
    int cnt_elems;
} generic_queue_t;

void GENQ_init(generic_queue_t *, size_t);
bool GENQ_is_empty(generic_queue_t*);
bool GENQ_is_full(generic_queue_t*);
bool GENQ_pop(generic_queue_t*, void *);
bool GENQ_push(generic_queue_t*, void *);

#endif