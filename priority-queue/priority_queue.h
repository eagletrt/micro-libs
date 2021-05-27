/**
 * @file priority_queue.h
 * @author Alessandro Sartori
 * @author Matteo Bonora
 *
 * @brief A versatile priority queue to store any type of payload.
 *
 * At initialization time the user provides a maximum queue length to allow the
 * preallocation of nodes (for efficiency's sake) and the size of one element (to
 * correctly setup the insert operations). Secondly, a compare function and a post-pop
 * function are needed: the former is used to determine which element has higher
 * priority among two, while the latter can be used to, for instance, decrement each
 * element's priority by one unit at each pop operation to avoid starvation effects.
 * */

#ifndef PRIO_Q_H
#define PRIO_Q_H

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

typedef uint16_t PQ_PriorityTypeDef;
typedef struct _PQ_QueueTypeDef *PQ_QueueTypeDef;

typedef int16_t (*PQ_cmp_priorities_fn)(PQ_PriorityTypeDef, PQ_PriorityTypeDef);
typedef void (*PQ_after_pop_fn)(PQ_PriorityTypeDef *);

void PQ_init(PQ_QueueTypeDef *, size_t, size_t, PQ_cmp_priorities_fn, PQ_after_pop_fn);
void PQ_destroy(PQ_QueueTypeDef *);
bool PQ_is_empty(PQ_QueueTypeDef);
bool PQ_insert(PQ_QueueTypeDef queue, PQ_PriorityTypeDef priority, void *payload);
void *PQ_peek_highest(PQ_QueueTypeDef);
void PQ_pop_highest(PQ_QueueTypeDef, void*);

#endif