/**
 * @file ucli.c
 * @brief 
 *
 * @date 04 Apr 2024
 * @author Name Surname [your@email.here]
 */

#include "ucli.h"

// Includes

#include <string.h>
#include <stdio.h>

// #define CTRL_C 3
// #define BACKSPACE 8
// #define LINE_FEED 10
// #define CARRIAGE_RETURN 13
#define UPPER_CASE_ALPHABET_START 65
#define UPPER_CASE_ALPHABET_END (65 + 25)
#define LOWER_CASE_ALPHABET_START (UPPER_CASE_ALPHABET_START + 32)
#define LOWER_CASE_ALPHABET_END (UPPER_CASE_ALPHABET_START + 25)
#define VALID_CHAR_START 32
#define VALID_CHAR_END 126

UART_HandleTypeDef* ucli_huart;
//no bene

// usare un handler per la funzione send, oppure attribute weak

ucli_state_t ucli_state = UCLI_STATE_INIT;

void ucli_init(UART_HandleTypeDef* huart) {
    //send welcome message
    ucli_huart = huart;

    ucli_state = ucli_run_state(ucli_state, NULL);
}

void ucli_routine(uint8_t byte) {
    if (byte != SPECIAL_CHAR_NULL)
    {
        #if ENABLE_ECHO
            ucli_send_message(&byte, 1);   
        #endif

        ucli_event_data_t event = { byte };
        ucli_event_trigger(&event); //questo trigger deve essere esterno. funzione

        ucli_state = ucli_run_state(ucli_state, NULL);
    }
}

bool ucli_is_valid_char(uint8_t byte) {
    if (byte >= VALID_CHAR_START && byte <= VALID_CHAR_END) {
        return true;
    } else {
        return false;
    }
}

bool ucli_is_valid_special_char(uint8_t byte) {
    switch (byte) {
        case SPECIAL_CHAR_NULL:
        case SPECIAL_CHAR_CTRL_C:
        case SPECIAL_CHAR_BACKSPACE:
        case SPECIAL_CHAR_LINE_FEED:
        case SPECIAL_CHAR_CARRIAGE_RETURN:
            return true;
            break;
        
        default:
            return false;
            break;
    }
}

void ucli_send_backspace() {
    uint8_t backspace = SPECIAL_CHAR_BACKSPACE;
    uint8_t space = 32;
    HAL_UART_Transmit(ucli_huart, &space, 1, SERIAL_TIMEOUT);
    HAL_UART_Transmit(ucli_huart, &backspace, 1, SERIAL_TIMEOUT);
}

void ucli_send_message(uint8_t* message, uint8_t message_size) {
    HAL_UART_Transmit(ucli_huart, message, message_size, SERIAL_TIMEOUT);
}

void ucli_send_error(ucli_error_t error) {
    uint8_t tx_buffer;
    char fixed_message[] = "UCLI ERROR:";
    uint8_t fixed_message_len = sizeof(fixed_message);
    char* error_message;
    uint8_t error_message_len;
    uint8_t full_message_len;
    
    switch (error)
    {
    case ucli_error_full_buffer:
        error_message = "Buffer is full";
        break;
    
    default:
        break;
    }

    //UCLI_ERROR: message
    full_message_len = fixed_message_len + error_message_len;
    snprintf(tx_buffer, full_message_len, "\n%s %s\n", fixed_message, error_message);

    HAL_UART_Transmit(ucli_huart, tx_buffer, full_message_len, SERIAL_TIMEOUT);
}

void ucli_command_buffer_init(ucli_command_buffer_t* command_buffer) {
    for (uint8_t i = 0; i < COMMAND_BUFFER_LEN; i++)
    {
        command_buffer->data[i] = 0;
    }
    command_buffer->head = -1;
}

bool ucli_command_buffer_is_empty(ucli_command_buffer_t* command_buffer) {
    return (command_buffer->head == -1);
}

bool ucli_command_buffer_is_full(ucli_command_buffer_t* command_buffer) {
    return (command_buffer->head == (COMMAND_BUFFER_LEN - 1));
}

bool ucli_command_buffer_push(ucli_command_buffer_t* command_buffer, uint8_t new_char) {
    bool return_value = false;
    
    if (!ucli_command_buffer_is_full(command_buffer))
    {
        command_buffer->head++;
        command_buffer->data[command_buffer->head] = new_char;
        return_value = true;
    }

    return return_value;
}

uint8_t ucli_command_buffer_pop(ucli_command_buffer_t* command_buffer) {
    uint8_t return_value = -1;
    
    if (!ucli_command_buffer_is_empty(command_buffer))
    {
        return_value = command_buffer->data[command_buffer->head];
        command_buffer->head--;
    }

    return return_value;
}

void ucli_command_buffer_clean(ucli_command_buffer_t* command_buffer) {
    while (ucli_command_buffer_pop(command_buffer) != -1){}
}