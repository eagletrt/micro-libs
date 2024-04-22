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
#include <stdint.h>
#include "main.h"

// Features
#define ENABLE_ECHO 1 // deve essere modificabile a runtime

// Parameters
#define SERIAL_TIMEOUT 100
#define INPUT_BUFFER_LEN 10
#define OUTPUT_BUFFER_LEN 10
#define COMMAND_BUFFER_LEN 10

// use char insted of numbers

// control characters
typedef enum {
    SPECIAL_CHAR_NULL = 0,
    SPECIAL_CHAR_CTRL_C = 3,
    SPECIAL_CHAR_BACKSPACE = 8,
    SPECIAL_CHAR_LINE_FEED = 10,
    SPECIAL_CHAR_CARRIAGE_RETURN = 13,
    SPECIAL_CHAR_N = 5
} SPECIAL_CHAR;

typedef enum {
    ucli_error_full_buffer,
} ucli_error_t;

//errors define or stringe costanti con i puntatori. oppure array usando l'enum come indice

typedef struct {
    uint8_t data[COMMAND_BUFFER_LEN];
    int8_t head;
} ucli_command_buffer_t;

void ucli_init(UART_HandleTypeDef* huart);
void ucli_routine(uint8_t byte);
bool ucli_is_valid_char(uint8_t byte);
bool ucli_is_valid_special_char(uint8_t byte);
void ucli_send_backspace(void);
void ucli_send_echo(uint8_t byte);
void ucli_send_message(uint8_t* message, uint8_t message_size);
void ucli_send_error(ucli_error_t error);

void ucli_command_buffer_init(ucli_command_buffer_t* command_buffer);
bool ucli_command_buffer_is_empty(ucli_command_buffer_t* command_buffer);
bool ucli_command_buffer_is_full(ucli_command_buffer_t* command_buffer);
bool ucli_command_buffer_push(ucli_command_buffer_t* command_buffer, uint8_t new_char);
uint8_t ucli_command_buffer_pop(ucli_command_buffer_t* command_buffer);
void ucli_command_buffer_clean(ucli_command_buffer_t* command_buffer);

#endif  // UCLI_H
