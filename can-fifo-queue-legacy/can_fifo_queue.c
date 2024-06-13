#include "stdlib.h"
#include "can_fifo_queue.h"


typedef struct _CANFQ_QueueTypeDef {
	uint8_t head, tail;
	CAN_MessageTypeDef queue[CANFQ_SIZE];
} _CANFQ_QueueTypeDef;

/**
 * @brief Initialize a new FIFO queue
 * @param fifo The queue to initialize
 * */
void CANFQ_init(_CANFQ_QueueTypeDef ** fifo) {
    (*fifo) = malloc(sizeof(_CANFQ_QueueTypeDef));
	(*fifo)->head = (*fifo)->tail = 0;
}

/**
 * @brief Free the resources used by a FIFO queue
 * @param fifo The queue to destroy
 * */
void CANFQ_destroy(_CANFQ_QueueTypeDef ** fifo) {
    free(*fifo);
}

/**
 * @brief Checks if the queue is empty
 * @param fifo The queue to inspect
 * @retval True if empty, false otherwise
 * */
bool CANFQ_is_empty(_CANFQ_QueueTypeDef * fifo) {
	return fifo->head == fifo->tail;
}

/**
 * @brief Checks if the queue is full
 * @param fifo The queue to inspect
 * @retval True if full, false otherwise
 * */
bool CANFQ_is_full(_CANFQ_QueueTypeDef * fifo) {
	return (fifo->head + 1) % CANFQ_SIZE == fifo->tail;
}

/**
 * @brief Pop the oldest CAN message from the FIFO queue
 * @param fifo The FIFO queue from which to pop
 * @param msg Where to store the popped message
 * @return True on success, false on error (empty queue)
 * */
bool CANFQ_pop(_CANFQ_QueueTypeDef * fifo, CAN_MessageTypeDef * msg) {
	if (CANFQ_is_empty(fifo))
		return false;

	/* Copy node contents to output parameter */
	CAN_MessageTypeDef * curr = &fifo->queue[fifo->tail];
	msg->id = curr->id;
	msg->size = curr->size;
	msg->hcan  = curr->hcan;
	for (int i = 0; i < curr->size; i++)
		msg->data[i] = curr->data[i];
	
	/* Increment tail */
	fifo->tail = (fifo->tail + 1) % CANFQ_SIZE;
	return true;
}

/**
 * @brief Push a new CAN message into the FIFO queue
 * @param fifo The FIFO queue in which to push
 * @param msg The message to be pushed
 * @return True on success, false on error (full queue)
 * */
bool CANFQ_push(_CANFQ_QueueTypeDef * fifo, CAN_MessageTypeDef * msg) {
	if (CANFQ_is_full(fifo))
		return false;
	
	/* Copy the message contents into the queue node */
	CAN_MessageTypeDef * curr = &fifo->queue[fifo->head];
	curr->id = msg->id;
	curr->size = msg->size;
	curr->hcan = msg->hcan;
	for (int i = 0; i < msg->size; i++)
		curr->data[i] = msg->data[i];
	
	/* Increment head */
	fifo->head = (fifo->head + 1) % CANFQ_SIZE;
	return true;
}
