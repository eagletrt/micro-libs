/**
 * @file error_utils.h
 * @brief Error handling library targeted for microcontrollers
 * @details This library is independent from the type of microcontroller and abstraction library used
 * 
 * @date Sep 13, 2023
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#ifndef ERROR_UTILS_H
#define ERROR_UTILS_H

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Set an error using an integer instance
 * @attention Use this macro instead of the function to avoid possible problems
 * and to increase simplicity and clarity of the code
 * @details Useful to iterate over errors
 * 
 * @param handler The errors handler (see type ErrorUtilsHandler)
 * @param error The error type (type uint32_t, should be and enum value)
 * @param instance The instance of the error (type uint32_t)
 * @return True if the error is set succesfully
 * @return False otherwise
 */
#define ERROR_UTILS_SET_INT(handler, error, instance) error_utils_set(handler, error, (ErrorUtilsInstance){ .i = instance }, false)
/**
 * @brief Set an error using a constant string instance
 * @attention Use this macro instead of the function to avoid possible problems
 * and to increase simplicity and clarity of the code
 * @attention Avoid the use of non constant strings, prefer literals over variables
 * @details Useful to give meaning to an instance
 * 
 * @param handler The errors handler (see type ErrorUtilsHandler)
 * @param error The error type (type uint32_t, should be and enum value)
 * @param instance The instance of the error (type const char * const)
 * @return True if the error is set succesfully
 * @return False otherwise
 */
#define ERROR_UTILS_SET_STR(handler, error, instance) error_utils_set(handler, error, (ErrorUtilsInstance){ .s = instance }, true)
/**
 * @brief Reset an error using an integer instance
 * @attention Use this macro instead of the function to avoid possible problems
 * and to increase simplicity and clarity of the code
 * @details Useful to iterate over errors
 * 
 * @param handler The errors handler (see type ErrorUtilsHandler)
 * @param error The error type (type uint32_t, should be and enum value)
 * @param instance The instance of the error (type uint32_t)
 * @return True if the error is removed succesfully
 * @return False otherwise
 */
#define ERROR_UTILS_RESET_INT(handler, error, instance) error_utils_reset(handler, error, (ErrorUtilsInstance){ .i = instance }, false)
/**
 * @brief Reset an error using a constant string instance
 * @attention Use this macro instead of the function to avoid possible problems
 * and to increase simplicity and clarity of the code
 * @attention Avoid the use of non constant strings, prefer literals over variables
 * @details Useful to give meaning to an instance
 * 
 * @param handler The errors handler (see type ErrorUtilsHandler)
 * @param error The error type (type uint32_t, should be and enum value)
 * @param instance The instance of the error (type const char * const)
 * @return True if the error is removed succesfully
 * @return False otherwise
 */
#define ERROR_UTILS_RESET_STR(handler, error, instance) error_utils_reset(handler, error, (ErrorUtilsInstance){ .s = instance }, true)


/**
 * @brief Function callback used to get the current timestamp
 * @details This function does not depend on the unit of measurement used
 * 
 * @return uint32_t The current timestamp
 */
typedef uint32_t (* ErrorUtilsTimestampCallback)(void);
/**
 * @brief Function callback used to get the timeout given a certain error type
 * @attention The unit of measurement have to be the same as the timestamp
 * 
 * @param error The error type
 * @return uint32_t The timeout of the given error
 */
typedef uint32_t (* ErrorUtilsTimeoutCallback)(uint32_t);
/**
 * @brief Function callback used to update the handler which should expire the errors
 * @details Can be NULL
 * @details The time when the error should expire can be calculated with this formula:
 *     expire_time = timestamp + timeout
 * How long does it take for the error to expire can be calculated as follows:
 *     expire_delta_time = expire_time - current_time
 * If the delta time is negative the error has already expired
 * 
 * @param timestamp The time when the error was set
 * @param timeout How long does it take for the error to expire
 */
typedef void (* ErrorUtilsExpireUpdateCallback)(uint32_t, uint32_t);


/** @brief A single error instance */
typedef union {
    uint32_t i;
    const char * s;
} ErrorUtilsInstance;

/**
 * @brief A single running error
 * @attention For declaration use only, do not modify the values of this structure
 * unless you know what you are doing
 */
typedef struct {
    uint32_t error;
    ErrorUtilsInstance instance;
    uint32_t timestamp;
    size_t heap_id;
    bool is_expired : 1;
    bool is_running : 1;
    bool is_dead : 1; // If set to 1 the error cannot expire until it is reset to 0
    bool string_instance : 1; // If true the instance is a string, otherwise it is an integer
} ErrorUtilsRunningInstance;

/** @brief Error handler structure */
typedef struct {
    size_t running;  // Number of running errors
    size_t expired;  // Number of expired errors

    size_t buffer_size;
    ErrorUtilsRunningInstance * errors;

    size_t expiring_end;
    ErrorUtilsRunningInstance ** expiring;

    ErrorUtilsTimestampCallback get_timestamp;
    ErrorUtilsTimeoutCallback get_timeout;
    ErrorUtilsExpireUpdateCallback set_expire;
} ErrorUtilsHandler;


/**
 * @brief Initialize the error handler
 * @attention The 'errors' and 'expiring' arrays have to be the same length
 * 
 * @param handler The error handler structure
 * @param errors An array of errors (don't need to be initialized)
 * @param expiring An array of pointers to errors (don't need to be initialized)
 * @param buffer_size The size of the arrays
 * @param get_timestamp The timestamp callback function
 * @param get_timeout The timeout callback function
 * @param set_expire The expire update callback function
 */
void error_utils_init(
    ErrorUtilsHandler * handler,
    ErrorUtilsRunningInstance * errors,
    ErrorUtilsRunningInstance ** expiring,
    size_t buffer_size,
    ErrorUtilsTimestampCallback get_timestamp,
    ErrorUtilsTimeoutCallback get_timeout,
    ErrorUtilsExpireUpdateCallback set_expire
);
/**
 * @brief Set an error
 * @attention Use this if you can't read or you like pain and suffering (please don't)
 * 
 * @param handler The error handler structure
 * @param error The error type
 * @param instance The error instance
 * @param is_string True if the instance is a string, otherwise it is an integer
 * @return True if the error is set correctly
 * @return False otherwise
 */
bool error_utils_set(
    ErrorUtilsHandler * handler,
    uint32_t error,
    ErrorUtilsInstance instance,
    bool is_string
);
/**
 * @brief Reset an error
 * @attention Use this if you can't read or you like pain and suffering (please don't)
 * 
 * @param handler The error handler structure
 * @param error The error type
 * @param instance The error instance
 * @param is_string True if the instance is a string, otherwise it is an integer
 * @return True if the error is removed correctly
 * @return False otherwise
 */
bool error_utils_reset(
    ErrorUtilsHandler * handler,
    uint32_t error,
    ErrorUtilsInstance instance,
    bool is_string
);
/**
 * @brief Function to call when the first error to expire has effectively expired
 * 
 * @param handler The error handler structure
 */
void error_utils_expire_errors(ErrorUtilsHandler * handler);


#endif // ERROR_UTILS_H