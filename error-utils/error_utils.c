#include "error_utils.h"

#include <string.h>

/** @brief Invalid values used as default */
#define ERROR_UTILS_INVALID_INT UINT32_MAX
#define ERROR_UTILS_INVALID_STR ""

/** @brief Critical section entry and exit point */
#define CS_ENTER() do {            \
    if (handler->cs_enter != NULL) \
        handler->cs_enter();       \
    } while(0)
#define CS_EXIT() do {            \
    if (handler->cs_exit != NULL) \
        handler->cs_exit();       \
    } while(0)

/**
 * @brief Get the parent index of a node of the heap
 * @attention The heap is implemented as an array
 * 
 * @param index The index of the node in the heap
 * @return size_t The parent index
 */
#define ERROR_UTILS_HEAP_PARENT(index) (((index) - 1) / 2)
/**
 * @brief Get the left child index of a node of the heap
 * @attention The heap is implemented as an array
 * 
 * @param index The index of the node in the heap
 * @return size_t The parent index
 */
#define ERROR_UTILS_HEAP_LCHILD(index) (2 * (index) + 1)
/**
 * @brief Get the parent index of a node of the heap
 * @attention The heap is implemented as an array
 * 
 * @param index The index of the node in the heap
 * @return size_t The parent index
 */
#define ERROR_UTILS_HEAP_RCHILD(index) (2 * (index) + 2)

/**
 * @brief Get the delta time between the current time and the expire time of the error
 * 
 * @param handler The error handler structure
 * @param err A running error
 * @return int32_t The delta time
 */
static inline int32_t _error_utils_get_delta(ErrorUtilsHandler * handler, ErrorUtilsRunningInstance * err) {
    int32_t t = handler->get_timestamp();
    return (int32_t)err->timestamp + (int32_t)handler->get_timeout(err->error) - t;
}
/**
 * @brief Compare the delta time of two errors
 * 
 * @param handler The error handler structure
 * @param first The first running error
 * @param second The second running error
 * @return True if the delta time of the first error is less than the second
 * @return False otherwise
 */
static inline bool _error_utils_compare(ErrorUtilsHandler * handler, ErrorUtilsRunningInstance * first, ErrorUtilsRunningInstance * second) {
    return _error_utils_get_delta(handler, first) < _error_utils_get_delta(handler, second);
}
/**
 * @brief Check if an error is equal given a type and an instance
 * 
 * @param err A running error
 * @param error The error type
 * @param instance The error instance
 * @param is_string True if the instance is a string, otherwise is an integer
 * @return True if the error is equal
 * @return False otherwise
 */
static inline bool _error_utils_is_equal(ErrorUtilsRunningInstance * err, uint32_t error, ErrorUtilsInstance instance, bool is_string) {
    if (err->string_instance != is_string)
        return false;
    bool inst_cmp = err->string_instance ? strcmp(instance.s, err->instance.s) == 0 : instance.i == err->instance.i;
    return err->error == error && inst_cmp;
}

/******************************************************************************/
/***************************** Heap functions *********************************/
/******************************************************************************/

/**
 * @brief Get the error with the minimum delta time from the heap
 * 
 * @param handler The error handler structure
 * @return ErrorUtilsRunningInstance * A pointer to the error
 */
static ErrorUtilsRunningInstance * _error_utils_heap_min(ErrorUtilsHandler * handler) {
    return handler->expiring[0];
}
/**
 * @brief Swap two errors inside the heap
 * 
 * @param first The first running error
 * @param second The second running error
 */
static void _error_utils_heap_swap(ErrorUtilsRunningInstance ** first, ErrorUtilsRunningInstance ** second) {
    // Swap pointers
    ErrorUtilsRunningInstance * aux = *first;
    *first = *second;
    *second = aux;

    // Swap heap ids
    size_t aux_id = (*first)->heap_id;
    (*first)->heap_id = (*second)->heap_id;
    (*second)->heap_id = aux_id;
}
/**
 * @brief Insert a new error in the heap
 * 
 * @param handler The error handler structure
 * @param err The error to insert
 */
static void _error_utils_heap_insert(ErrorUtilsHandler * handler, ErrorUtilsRunningInstance * err) {
    // Check to avoid buffer overflow
    if (handler->expiring_end == handler->buffer_size)
        return;
    
    // Insert the error instance to the end of the heap
    err->heap_id = handler->expiring_end;
    handler->expiring[handler->expiring_end++] = err;

    // Restore heap properties
    size_t i = handler->expiring_end - 1;
    size_t parent = ERROR_UTILS_HEAP_PARENT(i);
    while (i != 0 && _error_utils_compare(handler, handler->expiring[i], handler->expiring[parent])) {
        // Swap errors
        _error_utils_heap_swap(handler->expiring + i, handler->expiring + parent);

        // Update indices
        i = parent;
        parent = ERROR_UTILS_HEAP_PARENT(i);
    }

    // Update error heap id
    err->heap_id = i;
}
/**
 * @brief Remove an error from the heap
 * 
 * @param handler The error handler structure
 * @param err The error to remove
 */
static void _error_utils_heap_remove(ErrorUtilsHandler * handler, ErrorUtilsRunningInstance * err) {
    size_t index = err->heap_id;
    // The error is not in the heap
    if (index == ERROR_UTILS_INVALID_INT)
        return;

    // Swap the error with the end of the heap
    if (index < handler->expiring_end - 1)
        _error_utils_heap_swap(handler->expiring + index, handler->expiring + handler->expiring_end - 1);

    bool up_heapify = _error_utils_compare(handler, handler->expiring[index], handler->expiring[handler->expiring_end - 1]);

    // Remove last element
    --handler->expiring_end;
    handler->expiring[handler->expiring_end]->heap_id = ERROR_UTILS_INVALID_INT;
    handler->expiring[handler->expiring_end] = NULL;

    if (index < handler->expiring_end) {
        // Up-heapify
        if (up_heapify) {
            // Restore heap properties
            size_t parent = ERROR_UTILS_HEAP_PARENT(index);
            while (index != 0 && _error_utils_compare(handler, handler->expiring[index], handler->expiring[parent])) {
                // Swap errors
                _error_utils_heap_swap(handler->expiring + index, handler->expiring + parent);

                // Update indices
                index = parent;
                parent = ERROR_UTILS_HEAP_PARENT(index);
            }
        }
        // Down-heapify
        else {
            size_t l = ERROR_UTILS_HEAP_LCHILD(index);
            size_t r = ERROR_UTILS_HEAP_RCHILD(index);
            size_t child = (r >= handler->expiring_end) ? l : (
                _error_utils_compare(handler, handler->expiring[l], handler->expiring[r]) ? l : r);

            // Until a leaf is reached (or the parent with only the left child)
            while (r < handler->expiring_end && _error_utils_compare(handler, handler->expiring[child], handler->expiring[index])) {
                // Swap errors
                _error_utils_heap_swap(handler->expiring + index, handler->expiring + child);

                // Update indices
                index = child;
                l = ERROR_UTILS_HEAP_LCHILD(index);
                r = ERROR_UTILS_HEAP_RCHILD(index);
                child = (r >= handler->expiring_end) ? l : (
                    _error_utils_compare(handler, handler->expiring[l], handler->expiring[r]) ? l : r);
            }
            // Check left child
            if (l < handler->expiring_end && _error_utils_compare(handler, handler->expiring[l], handler->expiring[index]))
                _error_utils_heap_swap(handler->expiring + index, handler->expiring + child);
        }
    }
}

/******************************************************************************/
/***************************** Hash functions *********************************/
/******************************************************************************/

/**
 * @brief MurmurHash3 non-cryptographic hash function suitable for general hash-based lookup
 * @details For details refer to https://en.wikipedia.org/wiki/MurmurHash
 */
static inline uint32_t murmur_32_scramble(uint32_t k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}
static uint32_t murmur3_32(const uint8_t * key, size_t len, uint32_t seed)
{
	uint32_t h = seed;
    uint32_t k;
    /* Read in groups of 4. */
    for (size_t i = len >> 2; i; i--) {
        // Here is a source of differing results across endiannesses.
        // A swap here has no effects on hash properties though.
        memcpy(&k, key, sizeof(uint32_t));
        key += sizeof(uint32_t);
        h ^= murmur_32_scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }
    /* Read the rest. */
    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }
    // A swap is *not* necessary here because the preceding loop already
    // places the low bytes in the low places according to whatever endianness
    // we use. Swaps only apply when the memory is copied in a chunk.
    h ^= murmur_32_scramble(k);
    /* Finalize. */
	h ^= len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}
/**
 * @brief Generate an hash value from a string using an error type as the key
 * 
 * @param error The error type
 * @param str The string to get the hash from
 * @return uint32_t The generated hash value
 */
static inline uint32_t _error_utils_hash_string(uint32_t error, const char * str) {
    return murmur3_32((uint8_t *)str, strlen(str), error);
}
/**
 * @brief Generate an hash value from a integer using an error type as the key
 * 
 * @param error The error type
 * @param val The integer to get the hash from
 * @return uint32_t The generated hash value
 */
static inline uint32_t _error_utils_hash_int(uint32_t error, uint32_t val) {
    return murmur3_32((uint8_t *)(&val), sizeof(uint32_t), error);
}
/**
 * @brief Generate an hash value from an error instance using an error type as the key
 * 
 * @param error The error type
 * @param inst The error instance
 * @param is_string True if the instance is a string, otherwise it is an integer
 * @param buffer_size The total buffer size of the hash table
 * @return uint32_t An hash value from 0 to buffer_size - 1
 */
static inline uint32_t _error_utils_hash(uint32_t error, ErrorUtilsInstance inst, bool is_string, const size_t buffer_size) {
    return (is_string ? _error_utils_hash_string(error, inst.s) : _error_utils_hash_int(error, inst.i)) % buffer_size;
}
/**
 * @brief Get the new index in the hash table from a start index with a given offset
 * @example If there is a collision on the index 3 of the array (of length 11)
 * the next index can be calculated with an offset of 1 as follow:
 *      next_index = (3 + 1 * 1) % 11 = 4
 * If there is another collision the next index can be calulated by adding 1 to the offset
 * and repeating the calculation:
 *      next_index = (next_index + 2 * 2) % 11 = 8
 * 
 * @param start The start index
 * @param offset The offset from the first element
 * @param buffer_size The total buffer size of the hash table
 * @return uint32_t The next index in the hash table
 */
static inline uint32_t _error_utils_hash_probe(uint32_t start, uint32_t offset, uint32_t buffer_size) {
    // Quadratic probing
    return (start + (offset * offset) % buffer_size) % buffer_size;
}
/**
 * @brief Check if the error in the hash table is a free spot
 * @details A free spot in the hash table is defined when the error is not running and has not expired
 * 
 * @param err A running error
 * @return True if the error is a free spot 
 * @return False otherwise
 */
static inline bool _error_utils_hash_is_free(ErrorUtilsRunningInstance * err) {
    return !(err->is_running || err->is_expired);
}


void error_utils_init(
    ErrorUtilsHandler * handler,
    ErrorUtilsRunningInstance * errors,
    ErrorUtilsRunningInstance ** expiring,
    size_t buffer_size,
    ErrorUtilsTimestampCallback get_timestamp,
    ErrorUtilsTimeoutCallback get_timeout,
    ErrorUtilsExpireUpdateCallback set_expire,
    ErrorUtilsExpireStopCallback stop_expire,
    ErrorUtilsCriticalSectionEnterCallback cs_enter,
    ErrorUtilsCriticalSectionExitCallback cs_exit)
{
    if (handler == NULL ||
        errors == NULL ||
        expiring == NULL ||
        get_timestamp == NULL ||
        get_timeout == NULL ||
        set_expire == NULL ||
        stop_expire == NULL)
        return;

    // Init handler
    handler->running = 0;
    handler->expired = 0;
    
    handler->buffer_size = buffer_size;
    handler->errors = errors;
    handler->expiring_end = 0;
    handler->expiring = expiring;
    
    handler->get_timestamp = get_timestamp;
    handler->get_timeout = get_timeout;
    handler->set_expire = set_expire;
    handler->stop_expire = stop_expire;
    
    handler->cs_enter = cs_enter;
    handler->cs_exit = cs_exit;

    // Init error instances
    for (size_t i = 0; i < handler->buffer_size; i++) {
        handler->errors[i].error = ERROR_UTILS_INVALID_INT;
        handler->errors[i].instance.i = ERROR_UTILS_INVALID_INT;
        handler->errors[i].is_expired = false;
        handler->errors[i].is_running = false;
        handler->errors[i].heap_id = ERROR_UTILS_INVALID_INT;
        handler->errors[i].timestamp = 0;
        handler->errors[i].string_instance = false;

        handler->expiring[i] = NULL;
    }

    // Set handler init flag
    handler->is_init = true;
}
bool error_utils_set(
    ErrorUtilsHandler * handler,
    uint32_t error,
    ErrorUtilsInstance instance,
    bool is_string)
{
    if (handler == NULL ||
        !handler->is_init ||
        (is_string && instance.s == NULL))
        return false;

    // Enter critical section
    CS_ENTER();

    // Get error instance index
    uint32_t index = _error_utils_hash(error, instance, is_string, handler->buffer_size);
    
    ErrorUtilsRunningInstance * err = (handler->errors + index);

    // Iterate until a free spot is found
    size_t off = 0;
    for (; off < handler->buffer_size && !_error_utils_hash_is_free(err); ++off) {
        // Return if the error is already running or expired
        if (_error_utils_is_equal(err, error, instance, is_string)) {
            // Exit from the critical section
            CS_EXIT();
            return true;
        }

        // Update index using quadratic probing
        index = _error_utils_hash_probe(index, off, handler->buffer_size);
        err = (handler->errors + index);
    }

    // Buffer is full
    if (off == handler->buffer_size) {
        // TODO: Find error that expire after the new one
        // Then: Swap
        // Else: return false;

        // Exit from the critical section
        CS_EXIT();
        return false;
    }

    // Update error info
    err->error = error;
    err->instance = instance;
    err->timestamp = handler->get_timestamp();
    err->is_running = true;
    err->is_expired = false;
    err->string_instance = is_string;

    // Update handler
    ++(handler->running);

    // Update heap
    _error_utils_heap_insert(handler, err);

    // Update error expire handler if the new error is the first that will expire
    if (_error_utils_heap_min(handler) == err)
        handler->set_expire(err->timestamp, handler->get_timeout(error));

    // Exit from the critical section
    CS_EXIT();
    return true;
}
bool error_utils_reset(
    ErrorUtilsHandler * handler,
    uint32_t error,
    ErrorUtilsInstance instance,
    bool is_string)
{
    if (handler == NULL ||
        !handler->is_init ||
        (is_string && instance.s == NULL))
        return false;

    // Enter critical section
    CS_ENTER();

    // Get error instance index
    uint32_t index = _error_utils_hash(error, instance, is_string, handler->buffer_size);
    
    ErrorUtilsRunningInstance * err = (handler->errors + index);

    // Iterate until the error is found or the buffer is full and the error is not present
    size_t off = 0;
    for (; off < handler->buffer_size; ++off) {
        // Check if the error is initialized
        if (_error_utils_hash_is_free(err)) {
            // Exit from the critical section
            CS_EXIT();
            return true;
        }

        // Check if the error is already set
        if (_error_utils_is_equal(err, error, instance, is_string)) {
            // Reset the error if it was set
            if (err->is_running)
                break;
            // Exit from the critical section
            CS_EXIT();
            return true;
        }

        // Update index
        index = _error_utils_hash_probe(index, off, handler->buffer_size);
        err = (handler->errors + index);
    }

    // Buffer is full of other errors
    if (off == handler->buffer_size) {
        // Exit from the critical section
        CS_EXIT();
        return true;
    }

    ErrorUtilsRunningInstance * prev_min = _error_utils_heap_min(handler);

    // Remove error from heap
    _error_utils_heap_remove(handler, err);

    ErrorUtilsRunningInstance * curr_min = _error_utils_heap_min(handler);

    // Stop the expire handler if there are no running errors
    if (handler->expiring_end == 0)
        handler->stop_expire();
    // Otherwise update the expire handler
    else if (prev_min != curr_min)
        handler->set_expire(curr_min->timestamp, handler->get_timeout(curr_min->error));

    // Update handler
    --(handler->running);

    // Update error
    err->is_expired = false;
    err->is_running = false;

    // Exit from the critical section
    CS_EXIT();
    return true;
}
// TODO: Expire multiple errors
void error_utils_expire_errors(ErrorUtilsHandler * handler) {
    if (handler == NULL || !handler->is_init)
        return;

    // Enter critical section
    CS_ENTER();

    // Get first error that has expired
    ErrorUtilsRunningInstance * prev_min = _error_utils_heap_min(handler);

    // Remove error from heap
    _error_utils_heap_remove(handler, prev_min);

    ErrorUtilsRunningInstance * curr_min = _error_utils_heap_min(handler);

    // Stop expire handler if there are no running errors
    if (handler->expiring_end == 0)
        handler->stop_expire();
    // Otherwise update the expire handler
    else if (handler->set_expire != NULL)
        handler->set_expire(curr_min->timestamp, handler->get_timeout(curr_min->error));

    // Update error info
    prev_min->is_expired = true;
    prev_min->is_running = false;

    // Update handler info
    ++(handler->expired);
    --(handler->running);

    // Exit from the critical section
    CS_EXIT();
}
size_t error_utils_running_count(ErrorUtilsHandler * handler) {
    if (handler == NULL)
        return 0;
    return handler->running;
}
size_t error_utils_expired_count(ErrorUtilsHandler * handler) {
    if (handler == NULL)
        return 0;
    return handler->expired;
}
bool error_utils_is_running(
    ErrorUtilsHandler * handler,
    uint32_t error,
    ErrorUtilsInstance instance,
    bool is_string)
{
    if (handler == NULL)
        return false;

    // Get error instance index
    uint32_t index = _error_utils_hash(error, instance, is_string, handler->buffer_size);
    ErrorUtilsRunningInstance * err = (handler->errors + index);

    // Iterate until the error is found or the buffer is full and the error is not present
    size_t off = 0;
    for (; off < handler->buffer_size; ++off) {
        // Check if the error is initialized
        if (_error_utils_hash_is_free(err)) {
            // Exit from the critical section
            CS_EXIT();
            return false;
        }

        // Check if the error is already set
        if (_error_utils_is_equal(err, error, instance, is_string)) {
            bool is_running = err->is_running;
            
            // Exit from the critical section
            CS_EXIT();
            return is_running;
        }

        // Update index
        index = _error_utils_hash_probe(index, off, handler->buffer_size);
        err = (handler->errors + index);
    }

    // Exit from the critical section
    CS_EXIT();
    return false;
}
bool error_utils_is_expired(
    ErrorUtilsHandler * handler,
    uint32_t error,
    ErrorUtilsInstance instance,
    bool is_string)
{
    if (handler == NULL)
        return false;

    // Get error instance index
    uint32_t index = _error_utils_hash(error, instance, is_string, handler->buffer_size);
    ErrorUtilsRunningInstance * err = (handler->errors + index);

    // Iterate until the error is found or the buffer is full and the error is not present
    size_t off = 0;
    for (; off < handler->buffer_size; ++off) {
        // Check if the error is initialized
        if (_error_utils_hash_is_free(err)) {
            // Exit from the critical section
            CS_EXIT();
            return false;
        }

        // Check if the error is already set
        if (_error_utils_is_equal(err, error, instance, is_string)) {
            bool is_expired = err->is_expired;
            
            // Exit from the critical section
            CS_EXIT();
            return is_expired;
        }

        // Update index
        index = _error_utils_hash_probe(index, off, handler->buffer_size);
        err = (handler->errors + index);
    }

    // Exit from the critical section
    CS_EXIT();
    return false;
}
