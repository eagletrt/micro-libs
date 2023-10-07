/**
 * @file error_utils.h
 * @brief Error handling library targeted for microcontrollers
 * @details This library is independent from the type of microcontroller and abstraction library used
 * 
 * @details In order to use this library it is needed to setup a few things:
 * 
 *  - Declare at least one error handler of type ErrorUtilsHandler (or more if you plan on using multiple)
 *    The initialization is done via the error_utils_init function so remember to
 *    call it JUST ONCE for each handler
 *  - Declare an array of type ErrorUtilsRunningInstance and an array of pointers of the same type
 *    and remember to pass them to the init function
 *    They have to be the EXACTLY THE SAME SIZE
 *    No initialization is needed
 *  - Define a function as described by the ErrorUtilsTimestampCallback typedef
 *    which need to return the time (use whathaver function from any library you prefer)
 *    Remember to pass it to the init function
 *  - Define a function as described by the ErrorUtilsTimeoutCallback typedef
 *    which need to return the time needed for the given error to expire
 *    The unit of measurement has to be THE SAME AS THE TIMESTAMP
 *    Remember to pass it to the init function
 *  - Define a function as described by the ErrorUtilsExpireUpdateCallback typedef
 *    which need to update the error expire handler (like a timer) based on the timestamp of the error
 *    and its timeout
 *    Remember to pass it to the init function
 *  - Define a function as described by the ErrorUtilsExpireStopCallback typedef
 *    which need to stop the error expire handler (like a timer)
 *    Remember to pass it to the init function
 *  - If this library is used in a multithreaded environment or in an interrupt
 *    driven system the ErrorUtilsCriticalSectionEnterCallback and the ErrorUtilsCriticalSectionExitCallback
 *    function callbacks have to be defined in order to prevent data races between the internal data structure
 *    used by the library
 *    In that case remember to pass them to the init function otherwise pass NULL
 *  - Remember to call error_utils_expire_errors function to effectively expire a single error
 *    (multiple error is a work in progress)
 * 
 *  - Enjoy!!! :)
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
 * @attention This library can stop the handler that should expire the errors,
 * so when this callback gets called the handler should be restarted, if it was stopped
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
/**
 * @brief Function callback used to stop the handler which should expire the errors
 * @details This function should stop the expire handler so that the expire update callback
 * does not get called when there are no running errors
 */
typedef void (* ErrorUtilsExpireStopCallback)(void);
/**
 * @brief Function callback used to enter a critical section
 * @details Can be NULL
 * @details This function is used by the library to avoid data races on the internal data structures
 * that are used when the error is set or reset, this behaviour can be caused by multithreaded programs
 * or interrupt driven systems like microcontrollers for example
 */
typedef void (* ErrorUtilsCriticalSectionEnterCallback)(void);
/**
 * @brief Function callback used to exit from a critical section
 * @details Can be NULL
 * @details This function is used by the library to avoid data races on the internal data structures
 * that are used when the error is set or reset, this behaviour can be caused by multithreaded programs
 * or interrupt driven systems like microcontrollers for example
 */
typedef void (* ErrorUtilsCriticalSectionExitCallback)(void);


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
    ErrorUtilsExpireStopCallback stop_expire;

    ErrorUtilsCriticalSectionEnterCallback cs_enter;
    ErrorUtilsCriticalSectionExitCallback cs_exit;

    bool is_init : 1; // Flag to check if the handler is initialized correctly
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
 * @param stop_expire The expire stop callback function
 * @param cs_enter The critical section enter point callback function
 * @param cs_exit The critical section exit point callback function
 */
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
    ErrorUtilsCriticalSectionExitCallback cs_exit
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
/**
 * @brief Get the number of currently running errors
 * 
 * @param handler The error handler structure
 * @return size_t The number of running errors
 */
size_t error_utils_running_count(ErrorUtilsHandler * handler);
/**
 * @brief Get the numer of expired errors
 *
 * @param handler The error handler structure
 * @return size_t The number of expired errors
 */
size_t error_utils_expired_count(ErrorUtilsHandler * handler);

#endif // ERROR_UTILS_H
