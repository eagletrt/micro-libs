#ifndef PRIORITY_QUEUE_HEAP_H
#define PRIORITY_QUEUE_HEAP_H

#include "inttypes.h"
#include "stdbool.h"
#include "string.h"

typedef struct{
    int pq_size;
    int elem_size;
    int tail;
    int elem_cnt;
    int max_elem;
    bool (*compare)(int, int);
    uint8_t *queue;
    int *priority;
} PQ;

/*
    DEFAULT COMPARE FUNCTIONS
*/

/* greater or equal */
bool PQ_GE(int a, int b); 
/* greater than */
bool PQ_GT(int a, int b);
/* less or equal */
bool PQ_LE(int a, int b); 
/* less then */
bool PQ_LT(int a, int b); 

/**
 *
 * @param pq pointer to the queue to initialize
 * @param pq_size size of the memory allocated in bytes(*queue)
 * @param elem_size size of the single element
 * @param queue pointer to the allocated memory
 * @param priority pointer to the allocated memory for the priority
 * @param compare pointer to the compare function, you can use the default compare functions (PQ_GE, PQ_GT,)
 * 
 * @warning the last member is used for swapping, if you need n elements remember to allocate n+1 elements
 */
void PQ_init(PQ *pq, int pq_size, int elem_size, uint8_t *queue, int *priority, bool (*compare)(int, int));

/**
 * 
 * @brief get the first element of the queue, O(1)
 * 
 * @param pq pointer to the queue
 * @param priority destination pointer for priority
 * @param element destination pointer for the element
*/
void PQ_top(PQ *pq, int *priority, uint8_t *element);

/**
 * 
 * @brief insert a new element on the queue, O(log(n))
 * 
 * @param pq pointer to the queue
 * @param priority the priority of the element to insert
 * @param element pointer to the element to insert (it will be copied)
 * 
 * @returns true if inserted successfully, false otherwise
*/
bool PQ_insert(PQ *pq, int priority, uint8_t *element);


/**
 * 
 * @brief get the first element of the queue and delete it, O(log(n))
 * 
 * @param pq pointer to the queue
 * @param priority destination pointer for priority
 * @param element destination pointer for the element
 * 
 * @returns true if popped successfully, false otherwise
*/
bool PQ_pop(PQ *pq, int *priority, uint8_t *element);

/**
 * 
 * @param pq pointer to the queue
 * 
 * @returns true if the queue is full, false otherwise
*/
bool PQ_is_full(PQ *pq);

/**
 * 
 * @param pq pointer to the queue
 * 
 * @returns true if the queue is full, false otherwise
*/
bool PQ_is_empty(PQ *pq);


/*
    PRIVATE METHODS
*/

void _PQ_heap_restore(PQ *pq, int i);
int _PQ_parent(int i);
int _PQ_left(int i);
int _PQ_right(int i);
void _PQ_swap(PQ *pq, int a, int b);


#endif