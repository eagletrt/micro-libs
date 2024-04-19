/**
 * @file ring-buffer.c
 * @brief Implementation of a circular buffer without dinamyc allocation
 * and interrupt safe
 *
 * @date 13 Mar 2024
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#include "ring-buffer.h"

#include <string.h>

void _ring_buffer_cs_dummy(void) { }

RingBufferReturnCode _ring_buffer_init(
    RingBufferInterface * buffer,
    size_t data_size,
    size_t capacity,
    void (* cs_enter)(void),
    void (* cs_exit)(void))
{
    if (buffer == NULL)
        return RING_BUFFER_NULL_POINTER;
    buffer->start = 0;
    buffer->size = 0;
    buffer->data_size = data_size;
    buffer->capacity = capacity;
    buffer->cs_enter = cs_enter != NULL ? cs_enter : _ring_buffer_cs_dummy;
    buffer->cs_exit = cs_exit != NULL ? cs_exit : _ring_buffer_cs_dummy;
    memset(&buffer->data, 0, capacity);
    return RING_BUFFER_OK;
}

bool _ring_buffer_is_empty(RingBufferInterface * buffer) {
    if (buffer == NULL)
        return true;
    return buffer->size == 0;
}

bool _ring_buffer_is_full(RingBufferInterface * buffer) {
    if (buffer == NULL)
        return false;
    return buffer->size >= buffer->capacity;
}

size_t _ring_buffer_size(RingBufferInterface * buffer) {
    if (buffer == NULL)
        return 0U;
    return buffer->size;
}

RingBufferReturnCode _ring_buffer_push_front(RingBufferInterface * buffer, void * item) {
    if (buffer == NULL || item == NULL)
        return RING_BUFFER_NULL_POINTER;

    buffer->cs_enter();

    if (buffer->size >= buffer->capacity) {
        buffer->cs_exit();
        return RING_BUFFER_FULL;
    }
    
    // Calculate index of the item in the buffer
    if (buffer->start == 0)
        buffer->start = buffer->capacity;
    --buffer->start;
    ++buffer->size;

    // Push item in the buffer 
    const size_t data_size = buffer->data_size;
    uint8_t * base = (uint8_t *)&buffer->data;
    memcpy(base + buffer->start * data_size, item, data_size);

    buffer->cs_exit();
    return RING_BUFFER_OK;
}

RingBufferReturnCode _ring_buffer_push_back(RingBufferInterface * buffer, void * item) {
    if (buffer == NULL || item == NULL)
        return RING_BUFFER_NULL_POINTER;

    buffer->cs_enter();

    if (buffer->size >= buffer->capacity) {
        buffer->cs_exit();
        return RING_BUFFER_FULL;
    }

    // Calculate index of the item in the buffer
    size_t cur = buffer->start + buffer->size;
    if (cur >= buffer->capacity)
        cur -= buffer->capacity;

    // Push item in the buffer
    const size_t data_size = buffer->data_size;
    uint8_t * base = (uint8_t *)&buffer->data;
    memcpy(base + cur * data_size, item, data_size);
    ++buffer->size;

    buffer->cs_exit();
    return RING_BUFFER_OK;
}

RingBufferReturnCode _ring_buffer_pop_front(RingBufferInterface * buffer, void * out) {
    if (buffer == NULL)
        return RING_BUFFER_NULL_POINTER;

    buffer->cs_enter();

    if (buffer->size == 0) {
        buffer->cs_exit();
        return RING_BUFFER_EMPTY;
    }

    // Pop the item from the buffer 
    if (out != NULL) {
        const size_t data_size = buffer->data_size;
        uint8_t * base = (uint8_t *)&buffer->data;
        memcpy(out, base + buffer->start * data_size, data_size);
    }

    // Update start and size
    ++buffer->start;
    if (buffer->start >= buffer->capacity)
        buffer->start = 0;
    --buffer->size;
    
    buffer->cs_exit();
    return RING_BUFFER_OK;
}

RingBufferReturnCode _ring_buffer_pop_back(RingBufferInterface * buffer, void * out) {
    if (buffer == NULL)
        return RING_BUFFER_NULL_POINTER;

    buffer->cs_enter();

    if (buffer->size == 0) {
        buffer->cs_exit();
        return RING_BUFFER_EMPTY;
    }

    // Pop the item from the buffer 
    if (out != NULL) {
        size_t cur = buffer->start + buffer->size - 1;
        if (cur >= buffer->capacity)
            cur -= buffer->capacity;
        const size_t data_size = buffer->data_size;
        uint8_t * base = (uint8_t *)&buffer->data;
        memcpy(out, base + cur * data_size, data_size);
    }
    --buffer->size;
    
    buffer->cs_exit();
    return RING_BUFFER_OK;

}

RingBufferReturnCode _ring_buffer_front(RingBufferInterface * buffer, void * out) {
    if (buffer == NULL || out == NULL)
        return RING_BUFFER_NULL_POINTER;

    buffer->cs_enter();

    if (buffer->size == 0) {
        buffer->cs_exit();
        return RING_BUFFER_EMPTY;
    }

    // Copy data
    const size_t data_size = buffer->data_size;
    uint8_t * base = (uint8_t *)&buffer->data;
    memcpy(out, base + buffer->start * data_size, data_size);
    
    buffer->cs_exit();
    return RING_BUFFER_OK;
}

RingBufferReturnCode _ring_buffer_back(RingBufferInterface * buffer, void * out) {
    if (buffer == NULL || out == NULL)
        return RING_BUFFER_NULL_POINTER;

    buffer->cs_enter();

    if (buffer->size == 0) {
        buffer->cs_exit();
        return RING_BUFFER_EMPTY;
    }

    // Copy data
    size_t cur = buffer->start + buffer->size - 1;
    if (cur >= buffer->capacity)
        cur -= buffer->capacity;
    const size_t data_size = buffer->data_size;
    uint8_t * base = (uint8_t *)&buffer->data;
    memcpy(out, base + cur * data_size, data_size);
    
    buffer->cs_exit();
    return RING_BUFFER_OK;
}

void * _ring_buffer_peek_front(RingBufferInterface * buffer) {
    if (buffer == NULL)
        return NULL;
    
    buffer->cs_enter();

    if (buffer->size == 0) {
        buffer->cs_exit();
        return NULL;
    }
    uint8_t * front = (uint8_t *)&buffer->data + buffer->start * buffer->data_size;

    buffer->cs_exit();
    return front;
}

void * _ring_buffer_peek_back(RingBufferInterface * buffer) {
    if (buffer == NULL)
        return NULL;

    buffer->cs_enter();

    if (buffer->size == 0) {
        buffer->cs_exit();
        return NULL;
    }
    
    // Calculate index of the element in the buffer
    size_t cur = buffer->start + buffer->size - 1;
    if (cur >= buffer->capacity)
        cur -= buffer->capacity;
    uint8_t * back = (uint8_t *)&buffer->data + cur * buffer->data_size;

    buffer->cs_exit();
    return back;
}

RingBufferReturnCode _ring_buffer_clear(RingBufferInterface * buffer) {
    if (buffer == NULL)
        return RING_BUFFER_NULL_POINTER;
    buffer->cs_enter();
    buffer->start = 0;
    buffer->size = 0;
    buffer->cs_exit();
    return RING_BUFFER_OK;
}

