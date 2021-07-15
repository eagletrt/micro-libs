/**
 * @file      priority_queue_fast_insert.c
 * @author    Simone Ruffini        [simone.ruffini@tutanota.com]
 * @author    Matteo Bitussi        [matteobitussi@gmail.com]
 * @date      2021-07-02
 * @updated
 * @ingroup
 * @prefix
 * 
 * @brief     Priority queue implemented with a fixed circular bufffer with insertst 
 *            in O(1)
 * 
 */

/* Includes ------------------------------------------------------------------*/
#include "priority_queue_fast_insert.h"

#include "assert.h"
#include "inttypes.h"
#include "stdlib.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct ypeDef {
    PQFI_PriorityTypeDef priority; /*!< Node priority */
    void *payload;                 /*!< Node payload, this value is a memcopy */
} _PQFI_NodeTypeDef;

typedef struct _PQFI_HandleTypeDef {
    _PQFI_NodeTypeDef *buffer;           /*!< Circular buffer used as priority queue list */
    uint16_t pq_length;                  /*!< Priority queue length */
    uint16_t head;                       /*!< Index to head (max priority node) */
    uint16_t tail;                       /*!< Index to tail (for insert) */
    bool buf_full;                       /*!< Buffer full */
    uint16_t sort_pivot;                 /*!< Buffer index for the sorting pivot. 
                                              This index represents the last node until which nodes are ordered */
    size_t payload_size;                 /*!< Size of a payload, used for allocation purposes */
    PQFI_CmpPrtyFnTypeDef cmp_prty_fn;   /*!< Priority compare function, provided by the user */
    PQFI_AntiStrvFnTypeDef anti_strv_fn; /*!< Priority queue anti starvation function, at each pop */

} _PQFI_HandleTypeDef;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief Advance a given cursor in the bounds of the priority queue circular buffer
 * 
 * @param cursor  Pointer to a cursor
 *        WARNING: this value must be bound between 0 and pq_length
 */
static void _advance_cursor(_PQFI_HandleTypeDef *hpqfi, uint16_t *cursor);
/**
 * @brief   Sort priority queue from sort_pivot index to tail 
 * @details This sorting sorts a snapshot of the queue from head to tail. 
 *          NOTE: if the queue changes during sorting (interrupt from producer 
 *          that adds a node) the sorting works but does not sort the new added nodes.
 * 
 * @param   hpqfi _PQFI_HandleTypeDef pointer
 * @param   head  Head index at the time of sorting
 * @param   tail  Tail index at the tilme of sorting
 */
static void _pq_sort(_PQFI_HandleTypeDef *hpqfi, uint16_t head, uint16_t tail);

/**
 * @brief Apply anti starvation to nodes in the priority queue
 * 
 * @param hpqfi _PQFI_HandleTypeDef pointer
 * @param head  Head index at the time of sorting
 * @param tail  Tail index at the tilme of sorting
 */
static void _apply_anti_starvation(_PQFI_HandleTypeDef *hpqfi, uint16_t head, uint16_t tail);
/* Exported functions --------------------------------------------------------*/

PQFI_HandleTypeDef PQFI_init(
    size_t queue_length, size_t payload_size, PQFI_CmpPrtyFnTypeDef cmp_fn, PQFI_AntiStrvFnTypeDef astrv_fn) {
    assert(queue_length != 0);
    assert(cmp_fn != NULL);

    _PQFI_HandleTypeDef *hpqfi;

    /* Initialize pointers to NULL */
    hpqfi = (_PQFI_HandleTypeDef *)malloc(sizeof(_PQFI_HandleTypeDef));
    if (hpqfi == NULL) {
        return NULL;
    }

    /* Allocate circular buffer */
    hpqfi->buffer = (_PQFI_NodeTypeDef *)calloc(queue_length, sizeof(_PQFI_NodeTypeDef));
    if (hpqfi->buffer == NULL) {
        return NULL;
    }
    hpqfi->pq_length = queue_length;

    /* Reset cirucluar buffer */
    hpqfi->head = hpqfi->tail = 0;
    hpqfi->buf_full           = false;

    hpqfi->payload_size = payload_size;

    /* Priority queue stuff */
    hpqfi->sort_pivot = hpqfi->tail;

    hpqfi->cmp_prty_fn  = cmp_fn;
    hpqfi->anti_strv_fn = astrv_fn;

    /* Pre-allocate all nodes */
    for (size_t i = 0; i < queue_length; i++) {
        hpqfi->buffer[i].payload = calloc(1, hpqfi->payload_size);
    }
    return hpqfi;
}

void PQFI_destroy(_PQFI_HandleTypeDef *hpqfi) {
    /* De-allocate all payloads */
    for (size_t i = 0; i < hpqfi->pq_length; i++) {
        free(hpqfi->buffer[i].payload);
    }

    /* Deallocate priority queue/buffer */
    free(hpqfi->buffer);

    /* Free the main struct */
    free(hpqfi);
}

bool PQFI_is_empty(_PQFI_HandleTypeDef *hpqfi) {
    assert(hpqfi != NULL);

    return (!hpqfi->buf_full) && (hpqfi->head == hpqfi->tail);
}

bool PQFI_is_full(_PQFI_HandleTypeDef *hpqfi) {
    assert(hpqfi != NULL);

    return hpqfi->buf_full;
}

bool PQFI_insert(_PQFI_HandleTypeDef *hpqfi, PQFI_PriorityTypeDef priority, void *payload) {
    assert(hpqfi != NULL);
    assert(payload != NULL);

    /* If the hpqfi is full return */
    if (hpqfi->buf_full)
        return false;

    /* Copy payload data in tail node */
    memcpy(hpqfi->buffer[hpqfi->tail].payload, payload, hpqfi->payload_size);
    hpqfi->buffer[hpqfi->tail].priority = priority;

    /* Advance tail */
    _advance_cursor(hpqfi, &hpqfi->tail);

    return true;
}

bool PQFI_pop(_PQFI_HandleTypeDef *hpqfi, void *payload) {
    assert(hpqfi != NULL);
    assert(payload != NULL);

    if (PQFI_is_empty(hpqfi))
        return false;
    volatile uint16_t head_cpy = hpqfi->head;
    volatile uint16_t tail_cpy = hpqfi->tail;

    /* Apply anti starvation to PQ */
    _apply_anti_starvation(hpqfi, head_cpy, tail_cpy);

    /* Redorder hpqfi before pop */
    _pq_sort(hpqfi, head_cpy, tail_cpy);

    memcpy(payload, hpqfi->buffer[hpqfi->head].payload, hpqfi->payload_size);
    _advance_cursor(hpqfi, &(hpqfi->head));

    return true;
}

/* Private functions ---------------------------------------------------------*/

static void _pq_sort(_PQFI_HandleTypeDef *hpqfi, uint16_t head, uint16_t tail) {
    while (hpqfi->sort_pivot != tail) {
        /* First non in order element */
        uint16_t node_not_sorted = hpqfi->sort_pivot;
        _advance_cursor(hpqfi, &node_not_sorted);

        /* If all nodes are sorted return */
        if (node_not_sorted == tail)
            return;

        uint16_t cursor = head;
        /* cmp_prty_fn(a,b) = 1 if a.prio > b.prio , cmp_prty_fn(a,b) = 0 if a.prio <= b.prio */
        while (hpqfi->cmp_prty_fn(hpqfi->buffer[node_not_sorted].priority, hpqfi->buffer[cursor].priority) == 0) {
            _advance_cursor(hpqfi, &cursor);
        }
        while (cursor != node_not_sorted) {
            _PQFI_NodeTypeDef tmp_node;
            /* Copy cursor node in tmp node */
            memcpy(&tmp_node, &hpqfi->buffer[cursor], sizeof(_PQFI_NodeTypeDef));
            /* Replace cursor with not_sorted_node */
            memcpy(&hpqfi->buffer[cursor], &hpqfi->buffer[node_not_sorted], sizeof(_PQFI_NodeTypeDef));
            /* Replace not_sorted_node with tmp */
            memcpy(&hpqfi->buffer[node_not_sorted], &tmp_node, sizeof(_PQFI_NodeTypeDef));

            _advance_cursor(hpqfi, &cursor);
        }
        /* Advance sort pivot */
        _advance_cursor(hpqfi, &(hpqfi->sort_pivot));
    }
}

static void _advance_cursor(_PQFI_HandleTypeDef *hpqfi, uint16_t *cursor) {
    if (cursor == &hpqfi->head) {
        hpqfi->buf_full = false;
        /* If the head was moved and the sort_pivot was on the head move that too */
        if (hpqfi->head == hpqfi->sort_pivot)
            hpqfi->sort_pivot = (hpqfi->sort_pivot + 1) % hpqfi->pq_length;
    }
    *cursor = (*cursor + 1) % hpqfi->pq_length;
    if (cursor == &hpqfi->tail) {
        hpqfi->buf_full = hpqfi->head == hpqfi->tail;
    }
}

static void _apply_anti_starvation(_PQFI_HandleTypeDef *hpqfi, uint16_t head, uint16_t tail) {
    if (hpqfi->anti_strv_fn != NULL) {
        uint16_t cursor = head;
        while (cursor != tail) {
            hpqfi->buffer[cursor].priority = hpqfi->anti_strv_fn(hpqfi->buffer[cursor].priority);
            _advance_cursor(hpqfi, &cursor);
        }
    }
}
