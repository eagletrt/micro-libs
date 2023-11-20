#include "priority_queue_heap.h"

int _PQH_parent(int i) { return (i-1)/2; }
int _PQH_left(int i)   { return 2*i+1;   }
int _PQH_right(int i)  { return 2*i+2;   }

bool PQH_GE(int a, int b) { return a >= b; }
bool PQH_GT(int a, int b) { return a  > b; }
bool PQH_LE(int a, int b) { return a <= b; }
bool PQH_LT(int a, int b) { return a  < b; }

void _PQH_swap(PQH *pq, int a, int b)
{
    int tmp = pq->priority[a];
    pq->priority[a] = pq->priority[b];
    pq->priority[b] = tmp;
    memcpy(pq->queue+pq->max_elem*pq->elem_size, pq->queue+a*pq->elem_size, pq->elem_size);
    memcpy(pq->queue+a*pq->elem_size, pq->queue+b*pq->elem_size, pq->elem_size);
    memcpy(pq->queue+b*pq->elem_size, pq->queue+pq->max_elem*pq->elem_size, pq->elem_size);
}

void PQH_init(PQH *pq, int pq_size, int elem_size, uint8_t *queue, int *priority, bool (*compare)(int, int))
{
    pq->pq_size = pq_size;
    pq->elem_size = elem_size;
    pq->queue = queue;
    pq->compare = compare;
    pq->elem_cnt = pq->tail = 0;
    pq->max_elem = pq->pq_size / pq->elem_size - 1;     //last element for swapping
    pq->priority = priority;
}

bool PQH_is_full(PQH *pq)  { return pq->elem_cnt == pq->max_elem; }
bool PQH_is_empty(PQH *pq) { return pq->elem_cnt == 0; }

bool PQH_insert(PQH *pq, int priority, uint8_t *element)
{
    if(PQH_is_full(pq)) return 0;

    memcpy(pq->priority+pq->tail, &priority, 4);
    memcpy(pq->queue+pq->tail*pq->elem_size, element, pq->elem_size);
    int i = pq->tail;
    pq->tail++;
    pq->elem_cnt++;
    while(i > 0 && pq->compare(pq->priority[_PQH_parent(i)], pq->priority[i]))
    {
        _PQH_swap(pq, _PQH_parent(i), i);
        i = _PQH_parent(i);
    }

    return 1;
}
void PQH_top(PQH *pq, int *priority, uint8_t *element)
{
    memcpy(element, pq->queue, pq->elem_size);
    memcpy(priority, pq->priority, 4);
}
bool PQH_pop(PQH *pq, int *priority, uint8_t *element)
{
    if(PQH_is_empty(pq)) return 0;

    PQH_top(pq, priority, element);
    _PQH_swap(pq, 0, pq->tail-1);
    pq->elem_cnt--;
    pq->tail--;
    _PQH_heap_restore(pq, 0);

    return 1;
}

void _PQH_heap_restore(PQH *pq, int i)
{
    int l = _PQH_left(i), r = _PQH_right(i);
    int best = i;
    if(l < pq->elem_cnt && pq->compare(pq->priority[i], pq->priority[l]))
        best = l;
    if(r < pq->elem_cnt && pq->compare(pq->priority[best], pq->priority[r]))
        best = r;
    if(best != i)
    {
        _PQH_swap(pq, i, best);
        _PQH_heap_restore(pq, best);
    }
}