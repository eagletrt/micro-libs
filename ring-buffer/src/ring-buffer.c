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

__attribute__((weak)) void ring_buffer_cs_enter(void) { }
__attribute__((weak)) void ring_buffer_cs_exit(void) { }


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

bool _ring_buffer_push_front(RingBufferInterface * buffer, void * item) {
    if (buffer == NULL || item == NULL)
        return false;

    ring_buffer_cs_enter();

    if (buffer->size >= buffer->capacity) {
        ring_buffer_cs_exit();
        return false;
    }
    
    // Calculate index of the item in the buffer
    if (buffer->start == 0)
        buffer->start = buffer->capacity;
    --buffer->start;
    ++buffer->size;

    // Push item in the heap 
    const size_t data_size = buffer->data_size;
    void * base = (void *)&buffer->data;
    memcpy(base + buffer->start * data_size, item, data_size);

    ring_buffer_cs_exit();
    return true;
}

bool _ring_buffer_push_back(RingBufferInterface * buffer, void * item) {
    if (buffer == NULL || item == NULL)
        return false;

    ring_buffer_cs_enter();

    if (buffer->size >= buffer->capacity) {
        ring_buffer_cs_exit();
        return false;
    }

    // Calculate index of the item in the buffer
    size_t cur = buffer->start + buffer->size;
    if (cur >= buffer->capacity)
        cur -= buffer->capacity;

    // Push item in the heap
    const size_t data_size = buffer->data_size;
    void * base = (void *)&buffer->data;
    memcpy(base + cur * data_size, item, data_size);
    ++buffer->size;

    ring_buffer_cs_exit();
    return true;
}

bool _ring_buffer_pop_front(RingBufferInterface * buffer, void * out) {
    if (buffer == NULL)
        return false;

    ring_buffer_cs_enter();

    if (buffer->size == 0) {
        ring_buffer_cs_exit();
        return false;
    }

    // Pop the item from the heap 
    if (out != NULL) {
        const size_t data_size = buffer->data_size;
        void * base = (void *)&buffer->data;
        memcpy(out, base + buffer->start * data_size, data_size);
    }

    // Update start and size
    ++buffer->start;
    if (buffer->start >= buffer->capacity)
        buffer->start = 0;
    --buffer->size;
    
    ring_buffer_cs_exit();
    return true;
}

bool _ring_buffer_pop_back(RingBufferInterface * buffer, void * out) {
    if (buffer == NULL)
        return false;

    ring_buffer_cs_enter();

    if (buffer->size == 0) {
        ring_buffer_cs_exit();
        return false;
    }

    // Pop the item from the heap 
    if (out != NULL) {
        size_t cur = buffer->start + buffer->size;
        if (cur >= buffer->capacity)
            cur -= buffer->capacity;
        const size_t data_size = buffer->data_size;
        void * base = (void *)&buffer->data;
        memcpy(out, base + cur * data_size, data_size);
    }
    --buffer->size;
    
    ring_buffer_cs_exit();
    return true;

}

bool _ring_buffer_front(RingBufferInterface * buffer, void * out) {
    if (buffer == NULL || out == NULL)
        return false;

    ring_buffer_cs_enter();

    if (buffer->size == 0) {
        ring_buffer_cs_exit();
        return false;
    }

    // Copy data
    const size_t data_size = buffer->data_size;
    void * base = (void *)&buffer->data;
    memcpy(out, base + buffer->start * data_size, data_size);
    
    ring_buffer_cs_exit();
    return true;
}

bool _ring_buffer_back(RingBufferInterface * buffer, void * out) {
    if (buffer == NULL || out == NULL)
        return false;

    ring_buffer_cs_enter();

    if (buffer->size == 0) {
        ring_buffer_cs_exit();
        return false;
    }

    // Copy data
    size_t cur = buffer->start + buffer->size;
    if (cur >= buffer->capacity)
        cur -= buffer->capacity;
    const size_t data_size = buffer->data_size;
    void * base = (void *)&buffer->data;
    memcpy(out, base + cur * data_size, data_size);
    
    ring_buffer_cs_exit();
    return true;
}

void * _ring_buffer_peek_front(RingBufferInterface * buffer) {
    if (buffer == NULL)
        return NULL;
    
    ring_buffer_cs_enter();

    if (buffer->size == 0) {
        ring_buffer_cs_exit();
        return NULL;
    }
    void * front = (void *)&buffer->data + buffer->start * buffer->data_size;

    ring_buffer_cs_exit();
    return front;
}

void * _ring_buffer_peek_back(RingBufferInterface * buffer) {
    if (buffer == NULL)
        return NULL;

    ring_buffer_cs_enter();

    if (buffer->size == 0) {
        ring_buffer_cs_exit();
        return NULL;
    }
    
    // Calculate index of the element in the buffer
    size_t cur = buffer->start + buffer->size - 1;
    if (cur >= buffer->capacity)
        cur -= buffer->capacity;
    void * back = (void *)&buffer->data + cur * buffer->data_size;

    ring_buffer_cs_exit();
    return back;
}

void _ring_buffer_clear(RingBufferInterface * buffer) {
    if (buffer == NULL)
        return;
    ring_buffer_cs_enter();
    buffer->start = 0;
    buffer->size = 0;
    ring_buffer_cs_exit();
}

