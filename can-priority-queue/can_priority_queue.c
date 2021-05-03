#include "stdlib.h"
#include "stddef.h"
#include "inttypes.h"

#include "can_priority_queue.h"


typedef uint16_t CANPQ_PriorityTypeDef;

typedef struct CANPQ_NodeTypeDef {
    CANPQ_PriorityTypeDef priority;
    CAN_MessageTypeDef msg;
    struct CANPQ_NodeTypeDef * next;
} CANPQ_NodeTypeDef;

typedef struct _CANPQ_QueueTypeDef {
    CANPQ_NodeTypeDef * head;
    CANPQ_NodeTypeDef * free_nodes;
} _CANPQ_QueueTypeDef;


CANPQ_NodeTypeDef * _CANPQ_get_free_node(_CANPQ_QueueTypeDef * queue) {
    if (queue->free_nodes == NULL)
        return NULL;
    
    /* Pop a node from the free list and return it */
    CANPQ_NodeTypeDef * n = queue->free_nodes;
    queue->free_nodes = n->next;
    n->next = NULL;

    return n;
}

void _CANPQ_free_node(_CANPQ_QueueTypeDef * queue, CANPQ_NodeTypeDef * node) {
    /* Add the node to the free list */
    node->next = queue->free_nodes;
    queue->free_nodes = node;
}

/**
 * @brief Initializes a new priority queue
 * @param queue The queue to initialize
 * */
void CANPQ_init(_CANPQ_QueueTypeDef ** queue) {
    /* Initialize pointers to NULL */
    (*queue) = malloc(sizeof(_CANPQ_QueueTypeDef));
    (*queue)->head = (*queue)->free_nodes = NULL;

    /* Pre-allocate all nodes */
    CANPQ_NodeTypeDef * cursor;
    for (uint32_t i = 0; i < CANPQ_SIZE; i++) {
        cursor = malloc(sizeof(CANPQ_NodeTypeDef));
        cursor->next = (*queue)->free_nodes;
        (*queue)->free_nodes = cursor;
    }
}

/**
 * @brief Frees all resources used by a priority queue
 * @param queue The queue to free
 * */
void CANPQ_destroy(_CANPQ_QueueTypeDef ** q) {
    _CANPQ_QueueTypeDef * queue = *q;
    CANPQ_NodeTypeDef * cursor;

    /* De-allocate all used nodes */
    while (queue->head != NULL) {
        cursor = queue->head;
        queue->head = queue->head->next;
        free(cursor);
    }

    /* De-allocate all nodes in the free list */
    while (queue->free_nodes != NULL) {
        cursor = queue->free_nodes;
        queue->free_nodes = queue->free_nodes->next;
        free(cursor);
    }

    /* Free the main struct */
    free(*q);
}

/**
 * @brief Check if the queue is empty
 * @param queue The queue to inspect
 * @retval True if empty, false otherwise
 * */
bool CANPQ_is_empty(_CANPQ_QueueTypeDef * queue) {
    return queue->head == NULL;
}

/**
 * @brief Insert a new message in the queue. The lower the ID,
 *        the higher its priority
 * @param queue The queue in which to insert the element
 * @param msg The message to enqueue
 * */
void CANPQ_insert(_CANPQ_QueueTypeDef * queue, CAN_MessageTypeDef * msg) {
    CANPQ_NodeTypeDef * new_node = _CANPQ_get_free_node(queue);

    /* Populate the new node */
    new_node->priority = msg->id;
    new_node->msg = *msg;

    if (queue->head == NULL || queue->head->priority > new_node->priority) {
        /* Insert the new node in front of the list */
        new_node->next = queue->head;
        queue->head = new_node;
    } else {
        /* Find the proper position */
        CANPQ_NodeTypeDef * cursor = queue->head;
        while (cursor->next != NULL && cursor->next->priority <= new_node->priority)
            cursor = cursor->next;

        /* Insert the element after the cursor */
        new_node->next = cursor->next;
        cursor->next = new_node;
    }
}

/**
 * @brief Return a pointer to the element with the highest
 *        priority without removing it
 * @param queue The queue from which to peek the message
 * @retval The item with the highest priority
 * */
CAN_MessageTypeDef * CANPQ_peek_highest(_CANPQ_QueueTypeDef * queue) {
    return CANPQ_is_empty(queue) ? NULL : &(queue->head->msg);
}

/**
 * @brief Remove the element with the highest priority
 * @param queue The queue from which to pull the message
 * */
void CANPQ_pop_highest(_CANPQ_QueueTypeDef * queue) {
    if (CANPQ_is_empty(queue))
        return;
    
    /* Pop and free the head */
    CANPQ_NodeTypeDef * to_free = queue->head;
    queue->head = queue->head->next;
    _CANPQ_free_node(queue, to_free);

    /* Decrement all priorities by 1 to avoid starvation */
    CANPQ_NodeTypeDef * cursor = queue->head;
    while (cursor != NULL) {
        if (cursor->priority > 0)
            cursor->priority--;
        cursor = cursor->next;
    }
}
