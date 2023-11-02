#include "generic_queue.h"

void GENQ_init(generic_queue_t *q, size_t size, size_t elem_size, uint8_t *queue)
{
    q->size = size;
    q->queue_elem_size = elem_size;
    q->tail = q->head = q->cnt_elems = 0;
    q->max_elems = q->size/q->queue_elem_size;
    q->queue = queue;
}
bool GENQ_is_empty(generic_queue_t *q)
{
    return q->cnt_elems == 0;
}
bool GENQ_is_full(generic_queue_t *q)
{
    return q->cnt_elems >= q->max_elems;
}
bool GENQ_pop(generic_queue_t *q, uint8_t *e)
{
    if(GENQ_is_empty(q)) return 0;
    q->cnt_elems--;

    memcpy(e, q->queue+q->head*q->queue_elem_size, q->queue_elem_size);
    q->head++;
    q->head %= q->max_elems;
    return 1;
}
bool GENQ_push(generic_queue_t *q, uint8_t *e)
{
    if(GENQ_is_full(q)) return 0;
    q->cnt_elems++;

    memcpy(q->queue+q->tail*q->queue_elem_size, e, q->queue_elem_size);
    q->tail++;
    q->tail %= q->max_elems;
    return 1;
}