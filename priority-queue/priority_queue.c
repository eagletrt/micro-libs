#include "priority_queue.h"

#include "inttypes.h"
#include "stdlib.h"
#include "string.h"

typedef struct PQ_NodeTypeDef {
    PQ_PriorityTypeDef priority;
    void *payload;
    struct PQ_NodeTypeDef *next;
} PQ_NodeTypeDef;

typedef struct _PQ_QueueTypeDef {
    PQ_NodeTypeDef *head;
    PQ_NodeTypeDef *free_nodes;
    size_t payload_size;
} _PQ_QueueTypeDef;

PQ_NodeTypeDef *_PQ_get_free_node(_PQ_QueueTypeDef *queue) {
    if (queue->free_nodes == NULL)
        return NULL;

    /* Pop a node from the free list and return it */
    PQ_NodeTypeDef *n = queue->free_nodes;
    queue->free_nodes = n->next;
    n->next           = NULL;

    return n;
}

void _PQ_free_node(_PQ_QueueTypeDef *queue, PQ_NodeTypeDef *node) {
    /* Add the node to the free list */
    node->next        = queue->free_nodes;
    queue->free_nodes = node;
}

/**
 * @brief Initializes a new priority queue
 * @param queue The queue to initialize
 * */
void PQ_init(_PQ_QueueTypeDef **queue, size_t payload_size) {
    /* Initialize pointers to NULL */
    (*queue)       = malloc(sizeof(_PQ_QueueTypeDef));
    (*queue)->head = (*queue)->free_nodes = NULL;

    (*queue)->payload_size = payload_size;

    /* Pre-allocate all nodes */
    PQ_NodeTypeDef *cursor;
    for (uint32_t i = 0; i < PQ_SIZE; i++) {
        cursor               = malloc((*queue)->payload_size);
        cursor->next         = (*queue)->free_nodes;
        (*queue)->free_nodes = cursor;
    }
}

/**
 * @brief Frees all resources used by a priority queue
 * @param queue The queue to free
 * */
void PQ_destroy(_PQ_QueueTypeDef **q) {
    _PQ_QueueTypeDef *queue = *q;
    PQ_NodeTypeDef *cursor;

    /* De-allocate all used nodes */
    while (queue->head != NULL) {
        cursor      = queue->head;
        queue->head = queue->head->next;
        free(cursor);
    }

    /* De-allocate all nodes in the free list */
    while (queue->free_nodes != NULL) {
        cursor            = queue->free_nodes;
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
bool PQ_is_empty(_PQ_QueueTypeDef *queue) {
    return queue->head == NULL;
}

/**
 * @brief Insert a new message in the queue. The lower the ID,
 *        the higher its priority
 * @param queue The queue in which to insert the element
 * @param payload The message to enqueue
 * */
void PQ_insert(_PQ_QueueTypeDef *queue, PQ_PriorityTypeDef priority, void *payload) {
    PQ_NodeTypeDef *new_node = _PQ_get_free_node(queue);

    /* Populate the new node */
    new_node->priority = priority;
    memcpy(&(new_node->payload), payload, queue->payload_size);

    if (queue->head == NULL || queue->head->priority > new_node->priority) {
        /* Insert the new node in front of the list */
        new_node->next = queue->head;
        queue->head    = new_node;
    } else {
        /* Find the proper position */
        PQ_NodeTypeDef *cursor = queue->head;
        while (cursor->next != NULL && cursor->next->priority <= new_node->priority)
            cursor = cursor->next;

        /* Insert the element after the cursor */
        new_node->next = cursor->next;
        cursor->next   = new_node;
    }
}

/**
 * @brief Return a pointer to the element with the highest
 *        priority without removing it
 * @param queue The queue from which to peek the message
 * @retval The item with the highest priority
 * */
void *PQ_peek_highest(_PQ_QueueTypeDef *queue) {
    return PQ_is_empty(queue) ? NULL : &(queue->head->payload);
}

/**
 * @brief Remove the element with the highest priority
 * @param queue The queue from which to pull the message
 * */
void PQ_pop_highest(_PQ_QueueTypeDef *queue) {
    if (PQ_is_empty(queue))
        return;

    /* Pop and free the head */
    PQ_NodeTypeDef *to_free = queue->head;
    queue->head             = queue->head->next;
    _PQ_free_node(queue, to_free);

    /* Decrement all priorities by 1 to avoid starvation */
    PQ_NodeTypeDef *cursor = queue->head;
    while (cursor != NULL) {
        if (cursor->priority > 0)
            cursor->priority--;
        cursor = cursor->next;
    }
}
