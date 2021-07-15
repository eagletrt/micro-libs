/**
 * @file      priority_queue_fast_insert.h
 * @author    Simone Ruffini [simone.ruffini@tutanota.com]
 * @author    Matteo Bitussi [matteobitussi@gmail.com]
 * @date      2021-07-02
 * @updated
 * @ingroup
 * @prefix
 * 
 * @brief     Priority queue implemented with a fixed circular bufffer and O(1) inserts 
 * 
 * 
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PQFI_H_
#define _PQFI_H_

/* Includes ------------------------------------------------------------------*/
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
typedef uint16_t PQFI_PriorityTypeDef;
typedef struct _PQFI_HandleTypeDef *PQFI_HandleTypeDef;

typedef bool (*PQFI_CmpPrtyFnTypeDef)(PQFI_PriorityTypeDef, PQFI_PriorityTypeDef);
typedef PQFI_PriorityTypeDef (*PQFI_AntiStrvFnTypeDef)(PQFI_PriorityTypeDef);

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initializes a new priority queue
 * @param hpqfi        Pointer to a PQFI_HandleTypeDef instance 
 * @param queue_length Maximun number of elements in the queue
 * @param payload_size Size of one element
 * @param cmp_fn       Compare function for priorities of a node
 *                     cmp_fn(a,b) == true if a.prio>b.prio, false otherwise
 * @param astrv_fn     Optional function that removes starvation from the queue.
 *                     This function operates on nodes priorites and returns
 *                     new ones. Returned priorities should promote nodes with 
 *                     lower priorities that could starve at the end of the queue.
 *                      
 * @return Memory location (pointer) to an initialized priority queue instance
 * */
PQFI_HandleTypeDef PQFI_init(
    size_t queue_length, size_t payload_size, PQFI_CmpPrtyFnTypeDef cmp_fn, PQFI_AntiStrvFnTypeDef astrv_fn);

/**
 * @brief Frees all resources used by a priority queue
 *  
 * @param hpqfi PQFI_HandleTypeDef instance
 * */
void PQFI_destroy(PQFI_HandleTypeDef hpqfi);

/**
 * @brief Check if the queue is empty
 * 
 * @param hpqfi PQFI_HandleTypeDef instance
 * 
 * @retval true if empty, false otherwise
 * */
bool PQFI_is_empty(PQFI_HandleTypeDef hpqfi);

/**
 * @brief Check if the queue is full
 * 
 * @param hpqfi PQFI_HandleTypeDef instance
 * 
 * @return true if full, false otherwise
 */
bool PQFI_is_full(PQFI_HandleTypeDef hpqfi);

/**
 * @brief Fast insert in priority queue
 * 
 * @param hpqfi    PQFI_HandleTypeDef instance
 * @param priority The item's priority
 * @param payload  The value to enque. 
 *                 WARNING: This value has to be payload_size big and not NULL
 * 
 * @return True if the payload was added to the queue else otherwise (probably 
 *         out of MEM)
 * */
bool PQFI_insert(PQFI_HandleTypeDef hpqfi, PQFI_PriorityTypeDef priority, void *payload);

/**
 * @brief Slow pop returns copy of highest priority node in the queue
 * 
 * @param hpqfi   The priority queue
 * @param payload Pointer to a payload type. A memcpy will be performed on 
 *                payload with the contents of the highest priority value 
 *                from the queue. 
 *                WARNING: memory pointed by payload must be non NULL and of 
 *                size sizeof(payload_type). It can be either from heap or stack.
 * 
 * @return true if the pop was successful, false otherwise
 */
bool PQFI_pop(PQFI_HandleTypeDef hpqfi, void *payload);

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private Macros -----------------------------------------------------------*/

#endif
