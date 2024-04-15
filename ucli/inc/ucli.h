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

// Features
#define ENABLE_ECHO 0

// Parameters
#define INPUT_UFFER_LEN 10
#define COMMAND_BUFFER_LEN 10

typedef enum {
    SPECIAL_CHAR_CTRL_C = 3,
    SPECIAL_CHAR_BACKSPACE = 8,
    SPECIAL_CHAR_LINE_FEED = 10,
    SPECIAL_CHAR_CARRIAGE_RETURN = 13,
    SPECIAL_CHAR_N = 4
} SPECIAL_CHAR;

typedef struct {
    uint8_t data[COMMAND_BUFFER_LEN];
    uint8_t head
} ucli_command_buffer_t;

void ucli_init(void);
void ucli_routine(uint8_t* byte);
bool ucli_is_valid_char(uint8_t byte);
bool ucli_is_valid_special_char(uint8_t byte);
void ucli_handle_backspace(void);
void ucli_handle_enter(void);

#endif  // UCLI_H

