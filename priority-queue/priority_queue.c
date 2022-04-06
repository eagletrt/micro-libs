#include "priority_queue.h"

#include "assert.h"
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

    PQ_cmp_priorities_fn prio_cmp_fn;
    PQ_after_pop_fn prio_op_fn;
} _PQ_QueueTypeDef;


/* Default compare function in case the user provides NULL */
int16_t _PQ_cmp_int_impl(PQ_PriorityTypeDef a, PQ_PriorityTypeDef b) {
    return b - a;
}
PQ_cmp_priorities_fn _PQ_cmp_int = _PQ_cmp_int_impl;

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
 * @param queue_length The maximun number of elements in the queue
 * @param payload_size The size of one element
 * @param cmp Function to compare priorities. It should return >0 if the 
 *            priority of the first element is higher than the second
 * @param op  Function to apply to every element's priority after each pop 
 *            (ex: decrement to prevent starvation)
 * */
void PQ_init(
    _PQ_QueueTypeDef **queue, size_t queue_length, size_t payload_size, PQ_cmp_priorities_fn cmp, PQ_after_pop_fn op) {
    //assert(cmp != NULL); //commented because nullish values are handled at line 66
    /* Initialize pointers to NULL */
    (*queue)       = malloc(sizeof(_PQ_QueueTypeDef));
    (*queue)->head = (*queue)->free_nodes = NULL;

    (*queue)->payload_size = payload_size;
    (*queue)->prio_cmp_fn  = cmp != NULL ? cmp : _PQ_cmp_int;
    (*queue)->prio_op_fn   = op;

    /* Pre-allocate all nodes */
    PQ_NodeTypeDef *cursor;
    for (size_t i = 0; i < queue_length; i++) {
        cursor               = (PQ_NodeTypeDef *)malloc(sizeof(PQ_NodeTypeDef));
        cursor->payload      = malloc((*queue)->payload_size);
        cursor->next         = (*queue)->free_nodes;
        (*queue)->free_nodes = cursor;
    }
}

/**
 * @brief Frees all resources used by a priority queue
 * @param q The queue to free
 * */
void PQ_destroy(_PQ_QueueTypeDef **q) {
    _PQ_QueueTypeDef *queue = *q;
    PQ_NodeTypeDef *cursor;

    /* De-allocate all used nodes */
    while (queue->head != NULL) {
        cursor      = queue->head;
        queue->head = queue->head->next;
        //free(cursor->payload);
        free(cursor);
    }

    /* De-allocate all nodes in the free list */
    while (queue->free_nodes != NULL) {
        cursor            = queue->free_nodes;
        queue->free_nodes = queue->free_nodes->next;
        //free(cursor->payload);
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
 * @param priority The item's priority
 * @param payload The message to enqueue
 * 
 * @return True if the payload was added to the queue else otherwise (probably 
 *         out of MEM)
 * */
bool PQ_insert(_PQ_QueueTypeDef *queue, PQ_PriorityTypeDef priority, void *payload) {
    PQ_NodeTypeDef *new_node = _PQ_get_free_node(queue);

    // No free nodes available, exit with error
    if (new_node == NULL)
        return false;

    /* Populate the new node */
    new_node->priority = priority;
    memcpy(&(new_node->payload), payload, queue->payload_size);

    if (queue->head == NULL || queue->prio_cmp_fn(new_node->priority, queue->head->priority) > 0) {
        /* Insert the new node in front of the list */
        new_node->next = queue->head;
        queue->head    = new_node;
    } else {
        /* Find the proper position */
        PQ_NodeTypeDef *cursor = queue->head;
        while (cursor->next != NULL && queue->prio_cmp_fn(cursor->next->priority, new_node->priority) >= 0)
            cursor = cursor->next;

        /* Insert the element after the cursor */
        new_node->next = cursor->next;
        cursor->next   = new_node;
    }
    return true;
}

/**
 * @brief Return a pointer to the element with the highest priority without 
 *        removing it
 * @param queue The queue from which to peek the message
 * @retval The item with the highest priority
 * */
void *PQ_peek_highest(_PQ_QueueTypeDef *queue) {
    return PQ_is_empty(queue) ? NULL : &(queue->head->payload);
}

/**
 * @brief Remove the element with the highest priority and copy its contents
 *        into the payload parameter (if not NULL)
 * 
 * @param queue The queue from which to pull the message
 * @param payload Where to store a copy of the popped node
 * */
void PQ_pop_highest(_PQ_QueueTypeDef *queue, void *payload) {
    if (PQ_is_empty(queue))
        return;
    
    if (payload != NULL)
        memcpy(payload, &queue->head->payload, queue->payload_size);

    /* Pop and free the head */
    PQ_NodeTypeDef *to_free = queue->head;
    queue->head             = queue->head->next;
    _PQ_free_node(queue, to_free);

    /* Apply the post-pop operation to all nodes */
    if (queue->prio_op_fn != NULL) {
        PQ_NodeTypeDef *cursor = queue->head;
        while (cursor != NULL) {
            queue->prio_op_fn(&cursor->priority);
            cursor = cursor->next;
        }
    }
}
