/**
 * @file ucli.h
 * @brief 
 *
 * @date 04 Apr 2024
 * @author Name Surname [your@email.here]
 */

#ifndef UCLI_H
#define UCLI_H

#include "ucli_fsm.h"

// Parameters
#define UCLI_BUFFER_LEN 10

// Control characters
typedef enum {
    CONTROL_CHAR_CTRL_C = 3, // Ctrl+C
    CONTROL_CHAR_BACKSPACE = '\b',
    CONTROL_CHAR_LINE_FEED = '\n',
    CONTROL_CHAR_CARRIAGE_RETURN = '\r',
} CONTROL_CHAR;

typedef enum {
    UCLI_ERROR_FULL_BUFFER,
    UCLI_ERROR_N,
} UCLI_ERRORS;

typedef struct {
    void (*send)(char* message, size_t size);
    bool echo;
} ucli_handler_t;

// === Public functions ===

void ucli_init(ucli_handler_t ucli_handler);
void ucli_routine(void);
void ucli_receive_data(char c);

// === Private functions ===

bool _ucli_is_printable_char(char c);
bool _ucli_is_control_char(char c);
void _ucli_send_message(char* message);
char* _ucli_get_error_message(UCLI_ERRORS error);
bool _ucli_get_echo_setting_status(void);

#endif  // UCLI_H
