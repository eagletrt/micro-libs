/**
 * @file errorlib.h
 * @brief Simple critical error management library
 *
 * @date 23 Aug 2024
 * @author Giacomo Mazzucchi [giacomo.mazzucchi@studenti.unitn.it]
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#ifndef ERRORLIB_H
#define ERRORLIB_H

#include <stdint.h>
#include <stddef.h>

// Type definition for the error groups and instances
typedef uint32_t errorlib_error_group_t;
typedef uint16_t errorlib_error_instance_t;

/**
 * @brief Return code for the errorlib functions
 */
typedef enum {
    ERRORLIB_OK,
    ERRORLIB_NULL_POINTER,
    ERRORLIB_INVALID_GROUP,
    ERRORLIB_INVALID_INSTANCE,
    ERRORLIB_EXPIRED
} ErrorLibReturnCode;

/**
 * @brief Status of a single error instance
 */
typedef enum {
    ERRORLIB_STATUS_UNSET,
    ERRORLIB_STATUS_RUNNING,
    ERRORLIB_STATUS_EXPIRED
} ErrorLibStatus;

/**
 * @brief Information about a single error
 *
 * @param group The error group
 * @param group The error instance number
 */
typedef struct {
    errorlib_error_group_t group;
    errorlib_error_instance_t instance;
} ErrorInfo;

/**
 * @brief Handler structure for the errors
 *
 * @param groups The total number of groups
 * @param instances The total number of instances
 * @param expired The number of expired errors
 * @param expired_error Info about the first expired error
 * @param errors A pointer to an array of array of error instances
 * @param instances_count An array containing the total number of instances of each group
 * @param thresholds An array where containing the number that has to be reached to treat an error as expired, for each group
 */
typedef struct {
    size_t groups;
    size_t instances;

    size_t expired;
    ErrorInfo expired_error;

    int32_t ** errors;
    const size_t * instances_count;
    const size_t * thresholds;
} ErrorLibHandler;

/**
 * @brief Initialize the error handler
 *
 * @param handler A pointer to the error handler structure
 * @param errors A pointer to the array of array of error instances
 * @param instances_count A pointer to the array of total number of instances for each group
 * @param thresholds A pointer to the array of thresholds for each group
 * @param groups The total number of groups
 *
 * @return ErrorLibReturnCode
 *     - ERRORLIB_NULL_POINTER if a NULL pointer is given as parameter
 *     - ERRORLIB_OK otherwise
 */
ErrorLibReturnCode errorlib_init(
    ErrorLibHandler * handler,
    int32_t ** errors,
    const size_t * instances_count,
    const size_t * thresholds,
    size_t groups
);

/**
 * @brief Mark an error instance as set
 * 
 * @param handler A pointer to the error handler structure
 * @param group The error group
 * @param instance The error instance
 *
 * @return ErrorLibReturnCode
 *     - ERRORLIB_NULL_POINTER if a NULL pointer is given as parameter
 *     - ERRORLIB_EXPIRED if at least an error is already expired
 *     - ERRORLIB_INVALID_GROUP if the given group does not exists
 *     - ERRORLIB_INVALID_INSTANCE if the given instance does not exists
 *     - ERRORLIB_OK otherwise
 */
ErrorLibReturnCode errorlib_error_set(
    ErrorLibHandler * handler,
    errorlib_error_group_t group,
    errorlib_error_instance_t instance
);

/**
 * @brief Reset an error instance counter to zero
 * 
 * @param handler A pointer to the error handler structure
 * @param group The error group
 * @param instance The error instance
 *
 * @return ErrorLibReturnCode
 *     - ERRORLIB_NULL_POINTER if a NULL pointer is given as parameter
 *     - ERRORLIB_EXPIRED if at least an error is already expired
 *     - ERRORLIB_INVALID_GROUP if the given group does not exists
 *     - ERRORLIB_INVALID_INSTANCE if the given instance does not exists
 *     - ERRORLIB_OK otherwise
 */
ErrorLibReturnCode errorlib_error_reset(
    ErrorLibHandler * handler,
    errorlib_error_group_t group,
    errorlib_error_instance_t instance
);

/**
 * @brief Get the number of expired errors
 *
 * @param handler A pointer to the error handler structure
 *
 * @return size_t The number of expired errors
 */
size_t errorlib_get_expired(ErrorLibHandler * handler);

/**
 * @brief Get the status of a single error instance
 *
 * @param handler A pointer to the error handler structure
 * @param group The error group
 * @param instance The error instance
 *
 * @return ErrorLibStatus The error status
 */
ErrorLibStatus errorlib_error_get_status(
    ErrorLibHandler * handler,
    errorlib_error_group_t group,
    errorlib_error_instance_t instance
);

/**
 * @brief Get info about the first expired error
 *
 * @param handler A pointer to the error handler structure
 *
 * @return ErrorInfo The error info
 */
ErrorInfo errorlib_get_expired_info(ErrorLibHandler * handler);

#endif  // ERRORLIB_H

