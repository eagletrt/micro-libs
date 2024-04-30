/**
 * @file ucli.c
 * @brief 
 *
 * @date 04 Apr 2024
 * @author Name Surname [your@email.here]
 */

#include "ucli.h"

// === Private Includes ===
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// === Private Defines ===
#define MAX_ERROR_MESSAGE_LEN 20

// === Global variables ===
ucli_state_t ucli_state;
ucli_handler_t handler;
char ucli_error_messages[UCLI_ERROR_N][MAX_ERROR_MESSAGE_LEN] = {
    "Buffer is full",
    "Unknown char",
    "Unknown error",
};

// === Public functions ===

void ucli_init(ucli_handler_t ucli_handler) {
    handler = ucli_handler;
    ucli_state = UCLI_STATE_INIT;
}

void ucli_routine(void) {
    ucli_state = ucli_run_state(ucli_state, NULL);
}

void ucli_receive_data(char c) {
    ucli_event_data_t event = { 
        .character = c
    };

    ucli_event_trigger(&event);
}

// === Private functions ===

void _ucli_send_message(char* message, size_t size) {
    handler.send(message, size);
}

void _ucli_send_error_message(UCLI_ERRORS error) {
    char* prefix = "[UCLI_ERROR]";
    char* error_message = ucli_error_messages[error];
    const char fmt[] = "\r\n%s: %s\r\n";
    int size = snprintf(NULL, 0, fmt, prefix, error_message);
    char message[size + 1]; // +1 for terminating null byte
    snprintf(message, sizeof message, fmt, prefix, error_message);

    _ucli_send_message(message, size);
}

bool _ucli_is_printable_char(char c) {
    if (c >= ' ' && c <= '~') {
        return true;
    } else {
        return false;
    }
}

bool _ucli_is_control_char(char c) {
    switch (c) {
        case CONTROL_CHAR_CTRL_C:
        case CONTROL_CHAR_BACKSPACE:
        case CONTROL_CHAR_LINE_FEED:
        case CONTROL_CHAR_CARRIAGE_RETURN:
            return true;
            break;
        
        default:
            return false;
            break;
    }
}

bool _ucli_get_echo_setting_status(void) {
    return handler.echo;
}

void _ucli_cs_enter(void) {
    handler.cs_enter();
}

void _ucli_cs_exit(void) {
    handler.cs_exit();
}