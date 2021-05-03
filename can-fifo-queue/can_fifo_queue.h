/**
 * @file can_fifo_queue.h
 * @author Alessandro Sartori
 * @brief A FIFO queue specifically designed to work with Fenice's CAN network.
 * 
 * The queue accepts items of type "CAN_MessageTypeDef", which is assumed to be a
 * struct containing three uint16 fields: id, size, and a vector of data values.
 * The queue guarantees First-In-First-Out priority to messages, meaning the first
 * item to be pushed, will be the first to be returned when invoking a pop.
 * */

#ifndef _CAN_FIFO_H
#define _CAN_FIFO_H

#include "inttypes.h"
#include "stdbool.h"


#ifdef CANFQ_TEST
    // This constant is supposed to be only defined when testing the library
    // to create a mock CAN_MessageTypeDef type as it not otherwise available.
    typedef struct {
        uint16_t id;
        uint16_t size;
        uint16_t data[8];
    } CAN_MessageTypeDef;
#else
    #include "can.h"
#endif


#define CANFQ_SIZE 100

typedef struct _CANFQ_QueueTypeDef * CANFQ_QueueTypeDef;

void CANFQ_init(CANFQ_QueueTypeDef *);
void CANFQ_destroy(CANFQ_QueueTypeDef *);
bool CANFQ_is_empty(CANFQ_QueueTypeDef);
bool CANFQ_is_full(CANFQ_QueueTypeDef);
bool CANFQ_pop(CANFQ_QueueTypeDef, CAN_MessageTypeDef *);
bool CANFQ_push(CANFQ_QueueTypeDef, CAN_MessageTypeDef *);

#endif