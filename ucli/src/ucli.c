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
#define MAX_ERROR_MESSAGE_PREFIX_LEN 15
#define MAX_ERROR_MESSAGE_LEN 20

// === Global variables ===
ucli_state_t ucli_state;
ucli_handler_t handler;
char ucli_error_messages[UCLI_ERROR_N][MAX_ERROR_MESSAGE_LEN] = {
    "Buffer is full"
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
    size_t size = MAX_ERROR_MESSAGE_PREFIX_LEN + MAX_ERROR_MESSAGE_LEN + 1;
    char message[size];

    snprintf(message, size, "\n\r%s: %s\n\r", prefix, error_message);
    size = strlen(message);

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