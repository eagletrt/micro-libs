/**
 * @file can_priority_queue.h
 * @author Alessandro Sartori
 * @brief A priority queue specifically designed to work with Fenice's CAN network.
 * 
 * Each message's priority is based on its CAN ID and the time it has been waiting in
 * queue, to avoid starvation. The queue accepts items of type "CAN_MessageTypeDef",
 * which is assumed to be a struct with at least an integer field named "id".
 * */

#ifndef CAN_PRIO_Q_H
#define CAN_PRIO_Q_H

#include "stdbool.h"
#include "stdint.h"


#ifdef CANPQ_TEST
    // This constant is supposed to be only defined when testing the library
    // to create a mock CAN_MessageTypeDef type as it not otherwise available.
    typedef struct {
        uint16_t id;
        uint16_t data;
    } CAN_MessageTypeDef;
#else
    #include "can.h"
#endif


#define CANPQ_SIZE 100

typedef struct _CANPQ_QueueTypeDef * CANPQ_QueueTypeDef;

void CANPQ_init(CANPQ_QueueTypeDef *);
void CANPQ_destroy(CANPQ_QueueTypeDef *);
bool CANPQ_is_empty(CANPQ_QueueTypeDef);
void CANPQ_insert(CANPQ_QueueTypeDef, CAN_MessageTypeDef *);
CAN_MessageTypeDef * CANPQ_peek_highest(CANPQ_QueueTypeDef);
void CANPQ_pop_highest(CANPQ_QueueTypeDef);

#endif