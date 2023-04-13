/**
 * @file error_utils.c
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
 * @link https://github.com/squadracorsepolito/stmlibs/blob/master/error_utils/error_utils.c
 */

#include "error_utils.h"
#include "timer-utils/timer_utils.h"

/**
 * @brief Check if the first expiry happens before the second
 * 
 * @param expiry1 The first expiry in ms
 * @param expiry2 The second expiry in ms
 * @return true If expiry1 happens before expiry2
 * @return false Otherwise
 */
bool _error_utils_is_before(uint32_t expiry1, uint32_t expiry2) {
    uint64_t e1 = expiry1, e2 = expiry2, now = HAL_GetTick();
    if (e1 < now)
        e1 += UINT32_MAX;
    if (e2 < now)
        e2 += UINT32_MAX;

    return e1 < e2;
}
/**
 * @brief Get how much time is left until expiry from now
 * 
 * @param expiry Expiry time in ms
 * @return uint32_t The delta time from now
 */
uint32_t _error_utils_get_delta_from_now(uint32_t expiry) {
    uint64_t e = expiry, now = HAL_GetTick();
    if (e < now)
        e += UINT32_MAX;
    
    return e - now;
}
/**
 * @brief Set the error timer
 * 
 * @param handle The error handler structure
 * @param expiry Time until the timer expires
 * @return HAL_StatusTypeDef The result of the operation
 */
HAL_StatusTypeDef _error_utils_set_timer(ERROR_UTILS_HandleTypeDef * handle, uint32_t expiry) {
    if (handle == NULL)
        return HAL_ERROR;
    
    uint32_t ticks = TIM_MS_TO_TICKS(handle->timer, _error_utils_get_delta_from_now(expiry)) - 1;

    if (ticks > TIM_GET_MAX_AUTORELOAD(handle->timer))
        return HAL_ERROR;

    __HAL_TIM_SetCounter(handle->timer, 0);
    __HAL_TIM_SetAutoreload(handle->timer, ticks);
    __HAL_TIM_CLEAR_IT(handle->timer, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(handle->timer);

    return HAL_OK;
}
/**
 * @brief Find the instance of the first running error that is about to expire
 * 
 * @param handle The error handler structure
 * @param error_out A pointer to the error of the found instance, NULL if not found (can be NULL)
 * @return ERROR_UTILS_InstanceTypeDef * A pointer to the error instance if found, NULL otherwise
 */
ERROR_UTILS_InstanceTypeDef * _error_utils_find_first_expiring(ERROR_UTILS_HandleTypeDef * handle,
    ERROR_UTILS_ErrorTypeDef ** error_out) {
    if (handle == NULL)
        return NULL;
    
    ERROR_UTILS_InstanceTypeDef * min_instance = NULL;
    if (error_out != NULL) *error_out = NULL;
    uint32_t min_expiry = UINT32_MAX;

    size_t count = handle->count;

    // Check each error instance
    for (size_t i = 0; count > 0 && i < handle->errors_length; i++) {
        ERROR_UTILS_ErrorTypeDef * error = &handle->errors[i];
        for (size_t j = 0; count > 0 && j < error->instances_length; j++) {
            ERROR_UTILS_InstanceTypeDef * instance = &error->instances[j];

            if (instance->is_triggered) {
                // Check if current instance expires before min_instance
                if (_error_utils_is_before(instance->expected_expiry_ms, min_expiry)) {
                    if (error_out != NULL) *error_out = error;
                    min_instance = instance;
                    min_expiry = instance->expected_expiry_ms;
                }
                --count;
            }
        }
    }

    return min_instance;
}
/**
 * @brief Find the first instance of the error that is about to expire and it sets the
 * timer based on the found error
 * 
 * @param handle The error handler structure
 * @return HAL_StatusTypeDef The result of the operation
 */
HAL_StatusTypeDef _error_utils_find_next_expiring_and_set_timer(ERROR_UTILS_HandleTypeDef * handle) {
    if (handle == NULL)
        return HAL_ERROR;

    ERROR_UTILS_ErrorTypeDef * error = NULL;
    ERROR_UTILS_InstanceTypeDef * instance =
        _error_utils_find_first_expiring(handle, error);
    
    // Set timer
    if (instance != NULL) {
        if (_error_utils_set_timer(handle, instance->expected_expiry_ms) != HAL_OK)
            return HAL_ERROR;
        handle->first_to_expire_error    = error;
        handle->first_to_expire_instance = instance;
    }
    else {
        handle->first_to_expire_error    = NULL;
        handle->first_to_expire_instance = NULL;
        HAL_TIM_Base_Stop_IT(handle->timer);
    }

    return HAL_OK;
}



HAL_StatusTypeDef error_utils_init(ERROR_UTILS_HandleTypeDef * handle,
    TIM_HandleTypeDef * tim,
    ERROR_UTILS_ErrorTypeDef * errors,
    size_t errors_length,
    ERROR_UTILS_CallbackTypeDef global_toggle_callback,
    ERROR_UTILS_CallbackTypeDef global_expiry_callback) {
    if (handle == NULL || tim == NULL || !IS_TIM_INSTANCE(tim->Instance) || errors == NULL)
        return HAL_ERROR;

    handle->timer = tim;
    handle->global_toggle_callback = global_toggle_callback;
    handle->global_expiry_callback = global_expiry_callback;
    handle->count = 0;

    handle->first_to_expire_error    = NULL;
    handle->first_to_expire_instance = NULL;

    handle->errors_length = errors_length;
    handle->errors = errors;

    // Init errors
    for (size_t i = 0; i < handle->errors_length; i++) {
        ERROR_UTILS_ErrorTypeDef * error = &(handle->errors[i]);

        if (TIM_MS_TO_TICKS(handle->timer, error->expiry_delay_ms) - 1 > TIM_GET_MAX_AUTORELOAD(handle->timer))
            return HAL_ERROR;

        // Init instances
        for (size_t j = 0; j < error->instances_length; j++) {
            error->instances[i].is_triggered       = false;
            error->instances[i].expected_expiry_ms = 0;
        }
    }

    return HAL_OK;
}

HAL_StatusTypeDef error_utils_error_set(ERROR_UTILS_HandleTypeDef * handle,
    size_t error_index,
    size_t instance_index) {
    if (handle == NULL)
        return HAL_ERROR;
    if (error_index >= handle->errors_length)
        return HAL_ERROR;

    ERROR_UTILS_ErrorTypeDef * error = &(handle->errors[error_index]);

    if (instance_index >= error->instances_length)
        return HAL_ERROR;

    ERROR_UTILS_InstanceTypeDef * instance = &(error->instances[instance_index]);

    // Set error instance if it's not running
    if (!instance->is_triggered) {
        instance->expected_expiry_ms = HAL_GetTick() + error->expiry_delay_ms;

        // Set timer if it's the next error that should be triggered
        if (handle->first_to_expire_instance == NULL ||
            !handle->first_to_expire_instance->is_triggered ||
            _error_utils_is_before(instance->expected_expiry_ms, handle->first_to_expire_instance->expected_expiry_ms)) {
            if (_error_utils_set_timer(handle, instance->expected_expiry_ms) != HAL_OK)
                return HAL_ERROR;

            handle->first_to_expire_error    = error;
            handle->first_to_expire_instance = instance;
        }

        instance->is_triggered = true;
        ++handle->count;

        // Call callbacks if possible
        if (error->toggle_callback != NULL)
            error->toggle_callback(error_index, instance_index);
       
        if (handle->global_toggle_callback != NULL)
            handle->global_toggle_callback(error_index, instance_index);
    }

    return HAL_OK;
}
HAL_StatusTypeDef error_utils_error_reset(ERROR_UTILS_HandleTypeDef * handle,
    size_t error_index,
    size_t instance_index) {
    if (handle == NULL)
        return HAL_ERROR;
    if (error_index >= handle->errors_length)
        return HAL_ERROR;

    ERROR_UTILS_ErrorTypeDef * error = &(handle->errors[error_index]);

    if (instance_index >= error->instances_length)
        return HAL_ERROR;

    ERROR_UTILS_InstanceTypeDef * instance = &(error->instances[instance_index]);

    // Reset error instance if it's running
    if (instance->is_triggered) {
        // Check if it's the first error that is about to expire
        if (instance != handle->first_to_expire_instance &&
            _error_utils_find_next_expiring_and_set_timer(handle) != HAL_OK)
            return HAL_ERROR;

        instance->is_triggered = false;
        --handle->count;

        // Call callbacks if possible
        if (error->toggle_callback != NULL)
            error->toggle_callback(error_index, instance_index);
        if (handle->global_toggle_callback != NULL)
            handle->global_toggle_callback(error_index, instance_index);
    }

    return HAL_OK;
}

size_t error_utils_get_count(ERROR_UTILS_HandleTypeDef * handle) {
    if (handle == NULL)
        return 0;
    return handle->count;
}

/**
 * @brief Timer elapsed callback function
 * @details Sets the timer based on the next running error that is about to expire if there is one
 * 
 * @param handle The error handler structure
 * @param tim The elapsed timer
 * @return HAL_StatusTypeDef The result of the operation
 */
HAL_StatusTypeDef ERROR_UTILS_TimerElapsedCallback(ERROR_UTILS_HandleTypeDef * handle, TIM_HandleTypeDef * tim) {
    if (handle == NULL || tim == NULL)
        return HAL_ERROR;
    
    if (handle->timer == tim) {
        ERROR_UTILS_ErrorTypeDef * error = handle->first_to_expire_error;
        ERROR_UTILS_InstanceTypeDef * instance = handle->first_to_expire_instance;

        // Check if there is a running error
        if (error != NULL && instance != NULL && instance->is_triggered) {
            instance->is_triggered = false;

            if (_error_utils_find_next_expiring_and_set_timer(handle) != HAL_OK)
                return HAL_ERROR;

            size_t error_index = error - handle->errors;
            size_t instance_index = instance - error->instances;
            // Call callbacks if possible
            if (error->expiry_callback != NULL)
                error->expiry_callback(error_index, instance_index);
            if (handle->global_expiry_callback != NULL)
                handle->global_expiry_callback(error_index, instance_index);
        }
    }
    return HAL_OK;
}
