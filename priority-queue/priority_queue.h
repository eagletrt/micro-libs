/**
 * @file priority_queue.h
 * @author Alessandro Sartori
 * @author Matteo Bonora
 *
 * @brief A priority queue specifically designed to work with Fenice's CAN
 * network.
 *
 * Each message's priority is based on its CAN ID and the time it has been
 * waiting in queue, to avoid starvation. The queue accepts items of type
 * "CAN_MessageTypeDef", which is assumed to be a struct with at least an
 * integer field named "id".
 * */

#ifndef PRIO_Q_H
#define PRIO_Q_H

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

#define PQ_SIZE 100

typedef uint16_t PQ_PriorityTypeDef;
typedef struct _PQ_QueueTypeDef *PQ_QueueTypeDef;

void PQ_init(PQ_QueueTypeDef *queue, size_t payload_size);
void PQ_destroy(PQ_QueueTypeDef *);
bool PQ_is_empty(PQ_QueueTypeDef);
void PQ_insert(PQ_QueueTypeDef queue, PQ_PriorityTypeDef priority,
			   void *payload);
void *PQ_peek_highest(PQ_QueueTypeDef);
void PQ_pop_highest(PQ_QueueTypeDef);

#endif