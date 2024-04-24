/**
 * @file ring-buffer.h
 * @brief Implementation of a circular buffer without dinamyc allocation
 * and interrupt safe
 *
 * @date 13 Mar 2024
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Type definiton for the ring buffer handler structure
 * @details As an example you can declare and initialize a new ring buffer structure
 * that contains 10 integers as follows:
 *      RingBuffer(int, 10) buf = ring_buffer_new(int, 10, NULL, NULL);
 *
 * @param TYPE The data type of the items
 * @param CAPACITY The maximum number of elements of the buffer
 *
 * @param start The index of the first item in the buffer
 * @param size The current number of elements in the buffer
 * @param data_size The size of a single element in bytes
 * @param capacity The maximum number of elements that the buffer can contain
 * @param cs_enter A pointer to a function that should avoid interrupts
 * to happen inside the critical section
 * @param cs_exit A pointer to a function that should restore the normal functionalities
 * of the interrupts and mark the end of a critical section
 */
#define RingBuffer(TYPE, CAPACITY) \
struct { \
    size_t start; \
    size_t size; \
    uint16_t data_size; \
    size_t capacity; \
    void (* cs_enter)(void); \
    void (* cs_exit)(void); \
    TYPE data[CAPACITY]; \
}

/**
 * @brief Ring buffer handler structure initialization
 * @attention The TYPE and CAPACITY parameters must be the same as the ones
 * used in the structure declaration above
 *
 * @details As an example you can declare and initialize a new ring buffer structure
 * that contains 10 integers as follows:
 *      RingBuffer(int, 10) buf = ring_buffer_new(int, 10, NULL, NULL);
 *
 * @details If the ring_buffer_init function is used this macro is not needed
 *
 * @param TYPE The data type of the items
 * @param CAPACITY The maximum number of elements of the buffer
 * @param CS_ENTER A pointer to a function that should manage a critical section (can be NULL)
 * @param CS_EXIT A pointer to a function that should exit a critical section (can be NULL)
 */
#define ring_buffer_new(TYPE, CAPACITY, CS_ENTER, CS_EXIT) \
{ \
    .start = 0, \
    .size = 0, \
    .data_size = sizeof(TYPE), \
    .capacity = CAPACITY, \
    .cs_enter = CS_ENTER != NULL ? CS_ENTER : _ring_buffer_cs_dummy, \
    .cs_exit = CS_EXIT != NULL ? CS_EXIT : _ring_buffer_cs_dummy, \
    .data = { 0 } \
}

/**
 * @brief Structure definition used to pass the buffer handler as a function parameter
 * @attention This function should not be used directly
 */
typedef struct {
    size_t start;
    size_t size;
    uint16_t data_size;
    size_t capacity;
    void (* cs_enter)(void);
    void (* cs_exit)(void);
    void * data;
} RingBufferInterface;

/**
 * @brief Enum with all the possible return codes for the ring buffer functions
 */
typedef enum {
    RING_BUFFER_OK,
    RING_BUFFER_NULL_POINTER,
    RING_BUFFER_EMPTY,
    RING_BUFFER_FULL
} RingBufferReturnCode;


/**
 * @brief Initialize the buffer
 * @attention The type and capacity parameters must be the same as the ones
 * used in the structure declaration above
 *
 * @details If the ring_buffer_new macro is used this function is not needed
 *
 * @param buffer The buffer hanler structure
 * @param type The type of the items
 * @param capacity The maximum number of elements of the buffer
 * @param cs_enter A pointer to a function that should manage a critical section (can be NULL)
 * @param cs_exit A pointer to a function that should exit a critical section (can be NULL)
 * @return RingBufferReturnCode
 *     - RING_BUFFER_NULL if the buffer handler is NULL
 *     - RING_BUFFER_OK otherwise
 */
#define ring_buffer_init(buffer, type, capacity, cs_enter, cs_exit) _ring_buffer_init((RingBufferInterface *)(buffer), sizeof(type), capacity, cs_enter, cs_exit)

/**
 * @brief Check if the buffer is empty
 *
 * @param buffer The buffer handler structure
 * @return True if the buffer is empty, false otherwise
 */
#define ring_buffer_is_empty(buffer) _ring_buffer_is_empty((RingBufferInterface *)(buffer))

/**
 * @brief Check if the buffer is full
 *
 * @param buffer The buffer handler structure
 * @return True if the buffer is full, false otherwise
 */
#define ring_buffer_is_full(buffer) _ring_buffer_is_full((RingBufferInterface *)(buffer))

/**
 * @brief Get the current number of elements in the buffer
 * 
 * @param buffer The buffer handler structure
 * @return size_t The buffer size
 */
#define ring_buffer_size(buffer) _ring_buffer_size((RingBufferInterface *)(buffer))

/**
 * @brief Insert an element af the start of the buffer
 *
 * @param buffer The buffer handler structure
 * @param item A pointer to the item to insert
 * @return RingBufferReturnCode
 *     - RING_BUFFER_NULL if the buffer handler or the item are NULL
 *     - RING_BUFFER_FULL if the buffer is full
 *     - RING_BUFFER_OK otherwise
 */
#define ring_buffer_push_front(buffer, item) _ring_buffer_push_front((RingBufferInterface *)(buffer), (void *)(item))

/**
 * @brief Insert an element af the end of the buffer
 *
 * @param buffer The buffer handler structure
 * @param item A pointer to the item to insert
 * @return RingBufferReturnCode
 *     - RING_BUFFER_NULL if the buffer handler or the item are NULL
 *     - RING_BUFFER_FULL if the buffer is full
 *     - RING_BUFFER_OK otherwise
 */
#define ring_buffer_push_back(buffer, item) _ring_buffer_push_back((RingBufferInterface *)(buffer), (void *)(item))

/**
 * @brief Remove an element from the front of the buffer
 * @details The 'out' parameter can be NULL
 *
 * @param buffer The buffer handler structure
 * @param out A pointer to a variable where the removed item is copied into
 * @return RingBufferReturnCode
 *     - RING_BUFFER_NULL if the buffer handler is NULL
 *     - RING_BUFFER_EMPTY if the buffer is empty
 *     - RING_BUFFER_OK otherwise
 */
#define ring_buffer_pop_front(buffer, out) _ring_buffer_pop_front((RingBufferInterface *)(buffer), (void *)(out))

/**
 * @brief Remove an element from the end of the buffer
 * @details The 'out' parameter can be NULL
 *
 * @param buffer The buffer handler structure
 * @param out A pointer to a variable where the removed item is copied into
 * @return RingBufferReturnCode
 *     - RING_BUFFER_NULL if the buffer handler is NULL
 *     - RING_BUFFER_EMPTY if the buffer is empty
 *     - RING_BUFFER_OK otherwise
 */
#define ring_buffer_pop_back(buffer, out) _ring_buffer_pop_back((RingBufferInterface *)(buffer), (void *)(out))

/**
 * @brief Get a copy of the element at the start of the buffer
 *
 * @param buffer The buffer handler structure
 * @param out A pointer to a variable where the item is copied into
 * @return RingBufferReturnCode
 *     - RING_BUFFER_NULL if the buffer handler or out are NULL
 *     - RING_BUFFER_EMPTY if the buffer is empty
 *     - RING_BUFFER_OK otherwise
 */
#define ring_buffer_front(buffer, out) _ring_buffer_front((RingBufferInterface *)(buffer), (void *)(out))

/**
 * @brief Get a copy of the element at the end of the buffer
 *
 * @param buffer The buffer handler structure
 * @param out A pointer to a variable where the item is copied into
 * @return RingBufferReturnCode
 *     - RING_BUFFER_NULL if the buffer handler or out are NULL
 *     - RING_BUFFER_EMPTY if the buffer is empty
 *     - RING_BUFFER_OK otherwise
 */
#define ring_buffer_back(buffer, out) _ring_buffer_back((RingBufferInterface *)(buffer), (void *)(out))

/**
 * @brief Get a pointer to the element at the start of the buffer
 * @attention Keep in mind that the content of the item can change even if the
 * pointer don't
 *
 * @param buffer The buffer handler structure
 * @return void * The item at the start of the buffer
 */
#define ring_buffer_peek_front(buffer) _ring_buffer_peek_front((RingBufferInterface *)(buffer))

/**
 * @brief Get a pointer to the element at the end of the buffer
 * @attention Keep in mind that the content of the item can change even if the
 * pointer don't
 *
 * @param buffer The buffer handler structure
 * @return void * The item at the end of the buffer
 */
#define ring_buffer_peek_back(buffer) _ring_buffer_peek_back((RingBufferInterface *)(buffer))

/**
 * @brief Clear the buffer removing all items
 * @details The actual data is not erased, only the size is modified
 *
 * @param buffer The buffer handler structure
 * @return RingBufferReturnCode
 *     - RING_BUFFER_NULL if the buffer handler is NULL
 *     - RING_BUFFER_OK otherwise
 */
#define ring_buffer_clear(buffer) _ring_buffer_clear((RingBufferInterface *)(buffer))

/******************************************/
/*   DO NOT USE THE FOLLOWING FUNCTIONS   */
/*         USE THE MACRO INSTEAD          */
/******************************************/

RingBufferReturnCode _ring_buffer_init(
    RingBufferInterface * buffer,
    size_t data_size,
    size_t capacity,
    void (* cs_enter)(void),
    void (* cs_exit)(void)
);
bool _ring_buffer_is_empty(RingBufferInterface * buffer);
bool _ring_buffer_is_full(RingBufferInterface * buffer);
size_t _ring_buffer_size(RingBufferInterface * buffer);
RingBufferReturnCode _ring_buffer_push_front(RingBufferInterface * buffer, void * item);
RingBufferReturnCode _ring_buffer_push_back(RingBufferInterface * buffer, void * item);
RingBufferReturnCode _ring_buffer_pop_front(RingBufferInterface * buffer, void * out);
RingBufferReturnCode _ring_buffer_pop_back(RingBufferInterface * buffer, void * out);
RingBufferReturnCode _ring_buffer_front(RingBufferInterface * buffer, void * out);
RingBufferReturnCode _ring_buffer_back(RingBufferInterface * buffer, void * out);
void * _ring_buffer_peek_front(RingBufferInterface * buffer);
void * _ring_buffer_peek_back(RingBufferInterface * buffer);
RingBufferReturnCode _ring_buffer_clear(RingBufferInterface * buffer);

// Function that substitute cs_enter and cs_exit if they are NULL
void _ring_buffer_cs_dummy(void);

#endif  // RING_BUFFER_H

