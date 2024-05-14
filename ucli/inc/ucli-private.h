#ifndef UCLI_INTERNAL_H
#define UCLI_INTERNAL_H

#include "ucli.h"

// === Private Includes ===

#include "ucli-dictionary.h"
#include <stddef.h>

// === Private Parameters ===

#define CONTROL_CHAR_CTRL_C 3 // Ctrl+C
#define CONTROL_CHAR_BACKSPACE '\b'
#define CONTROL_CHAR_LINE_FEED '\n'
#define CONTROL_CHAR_CARRIAGE_RETURN '\r'

// === Private Global Variables ===

typedef enum {
    UCLI_ERROR_FULL_BUFFER,
    UCLI_ERROR_UNKNOWN_CHAR,
    UCLI_ERROR_UNKNOWN,
    UCLI_ERROR_N,
} UCLI_ERRORS;

extern ucli_dictionary_t commands;

// === Private Functions ===

bool _ucli_is_printable_char(char c);
bool _ucli_is_control_char(char c);
void _ucli_send_message(char* message, size_t size);
char* _ucli_get_error_message(UCLI_ERRORS error);
void _ucli_send_error_message(UCLI_ERRORS error);
bool _ucli_get_echo_setting_status(void);
void _ucli_cs_enter(void);
void _ucli_cs_exit(void);

#endif // UCLI_INTERNAL_H