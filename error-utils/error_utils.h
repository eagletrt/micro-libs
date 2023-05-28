/**
 * @file error_utils.h
 * @brief This file provide a generic time based error management
 *
 * @date Apr 02, 2023
 * @author Federico Carbone [federico.carbone.sc@gmail.com]
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 * 
 * "THE BEER-WARE LICENSE" (Revision 69):
 * Squadra Corse firmware team wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy us a beer in return.
 * 
 * @link https://github.com/squadracorsepolito/stmlibs/blob/master/error_utils/error_utils.h
 */

#ifndef ERROR_UTILS_H
#define ERROR_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>

typedef void (* ERROR_UTILS_CallbackTypeDef) (size_t error_index, size_t instance_index);

/** @brief Instance of a running error */
typedef struct {
    bool is_triggered;
    uint32_t expected_expiry_ms;
} ERROR_UTILS_InstanceTypeDef;
/**
 * @brief Structure of an error
 * @details Can have multiple running instances
 */
typedef struct {
    ERROR_UTILS_CallbackTypeDef toggle_callback;
    ERROR_UTILS_CallbackTypeDef expiry_callback;

    uint32_t expiry_delay_ms;

    size_t instances_length;
    ERROR_UTILS_InstanceTypeDef * instances;
} ERROR_UTILS_ErrorTypeDef;
/** @brief Error handler */
typedef struct {
    TIM_HandleTypeDef * timer;
    ERROR_UTILS_CallbackTypeDef global_toggle_callback;
    ERROR_UTILS_CallbackTypeDef global_expiry_callback;
    size_t count;

    ERROR_UTILS_ErrorTypeDef * first_to_expire_error;
    ERROR_UTILS_InstanceTypeDef * first_to_expire_instance;

    size_t errors_length;
    ERROR_UTILS_ErrorTypeDef * errors;
} ERROR_UTILS_HandleTypeDef;


/**
 * @brief Initialize the error handler structure
 * 
 * @param handle The error handler structure
 * @param tim The timer used for error handling
 * @param errors An array of errors
 * @param errors_length The length of the errors array
 * @param global_toggle_callback A global toggle callback function
 * @param global_expiry_callback A global expiry callback function
 * @return HAL_StatusTypeDef The result of the operation
 */
HAL_StatusTypeDef error_utils_init(ERROR_UTILS_HandleTypeDef * handle,
    TIM_HandleTypeDef * tim,
    ERROR_UTILS_ErrorTypeDef * errors,
    size_t errors_length,
    ERROR_UTILS_CallbackTypeDef global_toggle_callback,
    ERROR_UTILS_CallbackTypeDef global_expiry_callback);

/**
 * @brief Sets an error instance
 * 
 * @param handle The error handler structure
 * @param error_index The index of the error in the errors array
 * @param instance_index The index of the instance in the instances array of the error
 * @return HAL_StatusTypeDef The result of the operation
 */
HAL_StatusTypeDef error_utils_error_set(ERROR_UTILS_HandleTypeDef * handle,
    size_t error_index,
    size_t instance_index);
/**
 * @brief Reset an error instance
 * 
 * @param handle The error handler structure
 * @param error_index The index of the error in the errors array
 * @param instance_index The index of the instance in the instances array of the error
 * @return HAL_StatusTypeDef The result of the operation
 */
HAL_StatusTypeDef error_utils_error_reset(ERROR_UTILS_HandleTypeDef * handle,
    size_t error_index,
    size_t instance_index);

/**
 * @brief Get the number of currently running errors
 * 
 * @param handle The error handler structure
 * @return size_t The number of running errors
 */
size_t error_utils_get_count(ERROR_UTILS_HandleTypeDef * handle);

/**
 * @brief Timer elapsed callback function
 * @details Sets the timer based on the next running error that is about to expire if there is one
 * 
 * @param handle The error handler structure
 * @param tim The elapsed timer
 * @return HAL_StatusTypeDef The result of the operation
 */
HAL_StatusTypeDef ERROR_UTILS_TimerElapsedCallback(ERROR_UTILS_HandleTypeDef * handle, TIM_HandleTypeDef * tim);

#endif // ERROR_UTILS_H
