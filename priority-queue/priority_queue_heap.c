#include "priority_queue_heap.h"

int _PQ_parent(int i) { return (i-1)/2; }
int _PQ_left(int i)   { return 2*i+1;   }
int _PQ_right(int i)  { return 2*i+2;   }

bool PQ_GE(int a, int b) { return a >= b; }
bool PQ_GT(int a, int b) { return a  > b; }
bool PQ_LE(int a, int b) { return a <= b; }
bool PQ_LT(int a, int b) { return a  < b; }

void _PQ_swap(PQ *pq, int a, int b)
{
    int tmp = pq->priority[a];
    pq->priority[a] = pq->priority[b];
    pq->priority[b] = tmp;
    memcpy(pq->queue+pq->max_elem*pq->elem_size, pq->queue+a*pq->elem_size, pq->elem_size);
    memcpy(pq->queue+a*pq->elem_size, pq->queue+b*pq->elem_size, pq->elem_size);
    memcpy(pq->queue+b*pq->elem_size, pq->queue+pq->max_elem*pq->elem_size, pq->elem_size);
}

void PQ_init(PQ *pq, int pq_size, int elem_size, uint8_t *queue, int *priority, bool (*compare)(int, int))
{
    pq->pq_size = pq_size;
    pq->elem_size = elem_size;
    pq->queue = queue;
    pq->compare = compare;
    pq->elem_cnt = pq->tail = 0;
    pq->max_elem = pq->pq_size / pq->elem_size - 1;     //last element for swapping
    pq->priority = priority;
}

bool PQ_is_full(PQ *pq)  { return pq->elem_cnt == pq->max_elem; }
bool PQ_is_empty(PQ *pq) { return pq->elem_cnt == 0; }

bool PQ_insert(PQ *pq, int priority, uint8_t *element)
{
    if(PQ_is_full(pq)) return 0;

    memcpy(pq->priority+pq->tail, &priority, 4);
    memcpy(pq->queue+pq->tail*pq->elem_size, element, pq->elem_size);
    int i = pq->tail;
    pq->tail++;
    pq->elem_cnt++;
    while(i > 0 && pq->compare(pq->priority[_PQ_parent(i)], pq->priority[i]))
    {
        _PQ_swap(pq, _PQ_parent(i), i);
        i = _PQ_parent(i);
    }

    return 1;
}
void PQ_top(PQ *pq, int *priority, uint8_t *element)
{
    memcpy(element, pq->queue, pq->elem_size);
    memcpy(priority, pq->priority, 4);
}
bool PQ_pop(PQ *pq, int *priority, uint8_t *element)
{
    if(PQ_is_empty(pq)) return 0;

    PQ_top(pq, priority, element);
    _PQ_swap(pq, 0, pq->tail-1);
    pq->elem_cnt--;
    pq->tail--;
    _PQ_heap_restore(pq, 0);

    return 1;
}

void _PQ_heap_restore(PQ *pq, int i)
{
    int l = _PQ_left(i), r = _PQ_right(i);
    int best = i;
    if(l < pq->elem_cnt && pq->compare(pq->priority[i], pq->priority[l]))
        best = l;
    if(r < pq->elem_cnt && pq->compare(pq->priority[best], pq->priority[r]))
        best = r;
    if(best != i)
    {
        _PQ_swap(pq, i, best);
        _PQ_heap_restore(pq, best);
    }
}