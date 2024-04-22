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

// === Private Defines ===
#define MAX_ERROR_MESSAGE_LEN 20
#define VALID_CHAR_START ' '
#define VALID_CHAR_END '~'

// === Global variables ===
ucli_state_t ucli_state;
ucli_handler_t handler;
char error_messages[UCLI_ERROR_N][MAX_ERROR_MESSAGE_LEN] = {
    "Buffer is full"
};

// === Public functions ===

void ucli_init(ucli_handler_t ucli_handler) {
    //send welcome message

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

void _ucli_send_message(char* message) {
    uint8_t size = strlen(message);
    handler.send(message, size);
}

char* _ucli_get_error_message(UCLI_ERRORS error) {
    // add prefix
    // get message
    // return
    return error_messages[error];
}

bool _ucli_is_printable_char(char c) {
    if (c >= VALID_CHAR_START && c <= VALID_CHAR_END) {
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