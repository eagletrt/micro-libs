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
 *      RingBuffer(int, 10) buf = ring_buffer_init(int, 10);
 *
 * @param TYPE The data type of the items
 * @param CAPACITY The maximum number of elements of the buffer
 *
 * @param start The index of the first item in the buffer
 * @param size The current number of elements in the buffer
 * @param data_size The size of a single element in bytes
 * @param capacity The maximum number of elements that the buffer can contain
 */
#define RingBuffer(TYPE, CAPACITY) \
struct { \
    size_t start; \
    size_t size; \
    uint16_t data_size; \
    size_t capacity; \
    TYPE data[CAPACITY]; \
}

/**
 * @brief Ring buffer handler structure initialization
 * @attention The TYPE and CAPACITY parameters must be the same as the ones
 * used in the structure declaration above
 * @details As an example you can declare and initialize a new ring buffer structure
 * that contains 10 integers as follows:
 *      RingBuffer(int, 10) buf = ring_buffer_init(int, 10);
 *
 * @param TYPE The data type of the items
 * @param CAPACITY The maximum number of elements of the buffer
 */
#define ring_buffer_init(TYPE, CAPACITY) \
{ \
    .start = 0, \
    .size = 0, \
    .data_size = sizeof(TYPE), \
    .capacity = CAPACITY, \
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
    void * data;
} RingBufferInterface;

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
 * @return bool True if the item is inserted correctly, false otherwise
 */
#define ring_buffer_push_front(buffer, item) _ring_buffer_push_front((RingBufferInterface *)(buffer), (void *)(item))

/**
 * @brief Insert an element af the end of the buffer
 *
 * @param buffer The buffer handler structure
 * @param item A pointer to the item to insert
 * @return bool True if the item is inserted correctly, false otherwise
 */
#define ring_buffer_push_back(buffer, item) _ring_buffer_push_back((RingBufferInterface *)(buffer), (void *)(item))

/**
 * @brief Remove an element from the front of the buffer
 * @details The 'out' parameter can be NULL
 *
 * @param buffer The buffer handler structure
 * @param out A pointer to a variable where the removed item is copied into
 * @return bool True if the item is removed correctly, false otherwise
 */
#define ring_buffer_pop_front(buffer, out) _ring_buffer_pop_front((RingBufferInterface *)(buffer), (void *)(out))

/**
 * @brief Remove an element from the end of the buffer
 * @details The 'out' parameter can be NULL
 *
 * @param buffer The buffer handler structure
 * @param out A pointer to a variable where the removed item is copied into
 * @return bool True if the item is removed correctly, false otherwise
 */
#define ring_buffer_pop_back(buffer, out) _ring_buffer_pop_back((RingBufferInterface *)(buffer), (void *)(out))

/**
 * @brief Get a copy of the element at the start of the buffer
 *
 * @param buffer The buffer handler structure
 * @param out A pointer to a variable where the item is copied into
 * @return bool True if the item is copied correctly, false otherwise
 */
#define ring_buffer_front(buffer, out) _ring_buffer_front((RingBufferInterface *)(buffer), (void *)(out))

/**
 * @brief Get a copy of the element at the end of the buffer
 *
 * @param buffer The buffer handler structure
 * @param out A pointer to a variable where the item is copied into
 * @return bool True if the item is copied correctly, false otherwise
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
 */
#define ring_buffer_clear(buffer) _ring_buffer_clear((RingBufferInterface *)(buffer))

/**
 * @brief Enter a critical section where multiple threads or interrupts
 * can cause race condition or similar problems
 * @attention This function is only declared here and should be define within the
 * project using this library
 */
void ring_buffer_cs_enter(void) __attribute__((weak));

/**
 * @brief Exit a critical section where multiple threads or interrupts
 * can cause race condition or similar problems
 * @attention This function is only declared here and should be define within the
 * project using this library
 */
void ring_buffer_cs_exit(void) __attribute__((weak));

/******************************************/
/*   DO NOT USE THE FOLLOWING FUNCTIONS   */
/*         USE THE MACRO INSTEAD          */
/******************************************/

bool _ring_buffer_is_empty(RingBufferInterface * buffer);
bool _ring_buffer_is_full(RingBufferInterface * buffer);
size_t _ring_buffer_size(RingBufferInterface * buffer);
bool _ring_buffer_push_front(RingBufferInterface * buffer, void * item);
bool _ring_buffer_push_back(RingBufferInterface * buffer, void * item);
bool _ring_buffer_pop_front(RingBufferInterface * buffer, void * out);
bool _ring_buffer_pop_back(RingBufferInterface * buffer, void * out);
bool _ring_buffer_front(RingBufferInterface * buffer, void * out);
bool _ring_buffer_back(RingBufferInterface * buffer, void * out);
void * _ring_buffer_peek_front(RingBufferInterface * buffer);
void * _ring_buffer_peek_back(RingBufferInterface * buffer);
void _ring_buffer_clear(RingBufferInterface * buffer);

#endif  // RING_BUFFER_H

