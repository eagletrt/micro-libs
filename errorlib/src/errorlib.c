/**
 * @file errorlib.c
 * @brief Simple critical error management library
 *
 * @date 23 Aug 2024
 * @author Giacomo Mazzucchi [giacomo.mazzucchi@studenti.unitn.it]
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 */

#include "errorlib.h"

#include <string.h>

ErrorLibReturnCode errorlib_init(
    ErrorLibHandler * handler,
    int32_t ** errors,
    const size_t * instances_count,
    const size_t * thresholds,
    size_t groups)
{
    if (handler == NULL)
        return ERRORLIB_NULL_POINTER;
    memset(handler, 0U, sizeof(*handler)); 
    handler->groups = groups;
    handler->errors = errors;
    handler->instances_count = instances_count;
    handler->thresholds = thresholds;
    for (size_t i = 0U; i < groups; ++i)
        handler->instances += handler->instances_count[i];
    return ERRORLIB_OK;
}

ErrorLibReturnCode errorlib_error_set(
    ErrorLibHandler * handler,
    errorlib_error_group_t group,
    errorlib_error_instance_t instance)
{
    if (handler == NULL)
        return ERRORLIB_NULL_POINTER;
    if (handler->expired > 0U)
        return ERRORLIB_EXPIRED;
    const size_t * instances = handler->instances_count;
    if (group >= handler->groups)
        return ERRORLIB_INVALID_GROUP;
    if (instance >= instances[group])
        return ERRORLIB_INVALID_INSTANCE;
    // Check if the error is expired
    if (++handler->errors[group][instance] >= (int32_t)handler->thresholds[group]) {
        ++handler->expired;
        handler->errors[group][instance] = -1;

        // Update the expired error info
        handler->expired_error.group = group;
        handler->expired_error.instance = instance;
    }
    return ERRORLIB_OK;
}

ErrorLibReturnCode errorlib_error_reset(
    ErrorLibHandler * handler,
    errorlib_error_group_t group,
    errorlib_error_instance_t instance)
{
    if (handler == NULL)
        return ERRORLIB_NULL_POINTER;
    if (handler->expired > 0U)
        return ERRORLIB_EXPIRED;
    const size_t * instances = handler->instances_count;
    if (group >= handler->groups)
        return ERRORLIB_INVALID_GROUP;
    if (instance >= instances[group])
        return ERRORLIB_INVALID_INSTANCE;
    handler->errors[group][instance] = 0U;
    return ERRORLIB_OK;
}

size_t errorlib_get_expired(ErrorLibHandler * handler) {
    if (handler == NULL)
        return 0U;
    return handler->expired;
}

ErrorLibStatus errorlib_error_get_status(
    ErrorLibHandler * handler,
    errorlib_error_group_t group,
    errorlib_error_instance_t instance)
{
    if (handler == NULL)
        return ERRORLIB_STATUS_UNSET;
    const size_t * instances = handler->instances_count;
    if (group >= handler->groups)
        return ERRORLIB_STATUS_UNSET;
    if (instance >= instances[group])
        return ERRORLIB_STATUS_UNSET;
    int32_t cnt = handler->errors[group][instance];
    if (cnt == 0)
        return ERRORLIB_STATUS_UNSET;
    if (cnt < 0 || cnt >= (int32_t)handler->thresholds[group])
        return ERRORLIB_STATUS_EXPIRED;
    return ERRORLIB_STATUS_RUNNING;
}

ErrorInfo errorlib_get_expired_info(ErrorLibHandler * handler) {
    if (handler == NULL)
        return (ErrorInfo){ 0U };
    return handler->expired_error;
}
