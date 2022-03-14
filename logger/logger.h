/**
 * @file      logging.h
 * @author    Alessandro Sartori [alex.sartori1997@gmail.com]
 * @date      2021-08-26
 * 
 * @brief     Logging helper library
 * 
 */

#ifndef LOGGING_H
#define LOGGING_H

#include "stdint.h"
#include "stdbool.h"


/**
 * @brief     Utilities to print floats when they are SBORATI IN CULO PD
 *            Use like so: printf("%d.%d", GET_INT_PART(f), GET_DEC_PART(f))
 */
#define LOG_GET_INT_PART(x) ((uint32_t)(x))                             /*< Get integer part of a float */
#define LOG_GET_DEC_PART(x) ((uint8_t)(((x)-LOG_GET_INT_PART(x))*100))  /*< Get two decimal digits of a float as an integer */


typedef enum {
    LOGLEVEL_DEBUG = 0U,    /*< Messages useful to trace execution */
    LOGLEVEL_INFO,          /*< Messages that inform of non-important events */
    LOGLEVEL_WARN,          /*< Messages stating minor errors have occurred */
    LOGLEVEL_ERR            /*< Messages reporting major or critical errors */
} LOG_LogLevelTypeDef;


/**
 * @brief     Initialize logging parameters
 * 
 * @param     loglevel    The log level above which messages will be printed
 * @param     show_lineno Before the message, print its generating file and line number
 * @param     show_prefix Before the message, show its log level (eg., debug, info, etc)
 * @param     show_tick   Before the message, print the time in ticks
 */
void LOG_init(LOG_LogLevelTypeDef, bool, bool, bool);

/**
 * @brief     Log a message with a corresponding informative level
 * @param     loglevel A `LOG_LogLevelTypeDef` defining the message type
 * @param     fmt A printf-style format string to create the message
 * @param     ... Format string parameters
 */
#define LOG_write(...) _LOG_write(__FILE__, __LINE__, __VA_ARGS__)
void _LOG_write(char*, uint32_t, LOG_LogLevelTypeDef, char*, ...);

/**
 * @brief     Print Fenice's logo in ASCIIart with a subtitle (e.g., the firmware purpose)
 * 
 * @param     subtitle A subtitle to add below the logo
 */
void LOG_print_fenice_logo(char*);


/**
 * @brief     Logging primitive called by each @ref LOG_write. 
 *            NOTE: This funcion is weakly defined, hence must be re-instanced by the user.               
 *            This function must use BSP's functions to log on UART,SD,EEPROM etc..
 * 
 * @param     txt A string message null terminated 
 */
void _LOG_write_raw(char * txt);

#endif
