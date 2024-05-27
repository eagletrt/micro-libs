/**
 * @file ucli.c
 * @brief Command Line Interface for embedded systems
 *
 * @date May 2024
 * @author Enrico Dalla Croce (Kalsifer-742) [kalsifer742@gmail.com]
 */

#include "ucli.h"

// === Private Includes ===

#include "ucli-fsm.h"
#include "ucli-private.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// === Global variables ===
ucli_state_t ucli_state;
ucli_handler_t handler;
ucli_dictionary_t commands;

// === Public functions ===

ucli_return_codes_t ucli_init(ucli_handler_t ucli_handler) {
    handler = ucli_handler;
    if (ucli_dictionary_init(&commands) != UCLI_DICTIONARY_RETURN_CODE_OK) {
        return UCLI_RETURN_CODE_ERROR;
    }
    ucli_state = UCLI_STATE_INIT;

    return UCLI_RETURN_CODE_OK;
}

ucli_return_codes_t ucli_routine(void) {
    ucli_state = ucli_run_state(ucli_state, NULL);
    if (ucli_is_event_managed()) {
        return UCLI_RETURN_CODE_OK;
    } else {
        return UCLI_RETURN_CODE_BUSY;
    }
}

ucli_return_codes_t ucli_receive_data(char c) {
    ucli_event_data_t event = {.data = c, .managed = false};

    ucli_event_trigger(event);

    return UCLI_RETURN_CODE_OK;
}

ucli_return_codes_t ucli_add_command(ucli_command_t command) {
    ucli_dictionary_return_code_t result =
        ucli_dictionary_add(&commands, command.name, command.function);

    if (result == UCLI_DICTIONARY_RETURN_CODE_FUN_PTR_IS_NULL) {
        return UCLI_RETURN_CODE_FUN_PTR_IS_NULL;
    } else if (result == UCLI_DICTIONARY_RETURN_CODE_FULL) {
        return UCLI_RETURN_CODE_MAX_COMMANDS_N_REACHED;
    }

    return UCLI_RETURN_CODE_OK;
}

// === Private functions ===

void _ucli_enable_receive(void) {
    handler.enable_receive();
}

void _ucli_send_message(char* message, size_t size) {
    handler.send(message, size);
}

void _ucli_send_prompt(void) { _ucli_send_message(UCLI_PROMPT, 2); }

char* _ucli_get_error_message(UCLI_ERRORS error) {
    static char* buffer_is_full_error_message = "Buffer is full";
    static char* unknown_char_error_message = "Unknown char";
    static char* unknown_command_error_message = "Unknown command";
    static char* unknown_error_message = "Unknown error";

    switch (error) {
    case UCLI_ERROR_FULL_BUFFER:
        return buffer_is_full_error_message;
        break;
    case UCLI_ERROR_UNKNOWN_CHAR:
        return unknown_char_error_message;
        break;
    case UCLI_ERROR_UNKNOWN_COMMAND:
        return unknown_command_error_message;
        break;

    default:
        return unknown_error_message;
        break;
    }
}

void _ucli_send_error_message(UCLI_ERRORS error) {
    char* prefix = "[UCLI_ERROR]";
    char* error_message = _ucli_get_error_message(error);
    const char fmt[] = "\r\n%s: %s\r\n";
    int size = snprintf(NULL, 0, fmt, prefix, error_message);
    char message[size + 1]; // +1 for terminating null byte
    snprintf(message, sizeof message, fmt, prefix, error_message);

    _ucli_send_message(message, size);
}

bool _ucli_is_printable_char(char c) { return c >= ' ' && c <= '~'; }

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

bool _ucli_get_echo_setting_status(void) { return handler.echo; }

void _ucli_cs_enter(void) {
    if (handler.cs_enter == NULL) {
        return;
    }
    handler.cs_enter();
}

void _ucli_cs_exit(void) {
    if (handler.cs_exit == NULL) {
        return;
    }
    handler.cs_exit();
}
