/**
 * @file ucli.c
 * @brief 
 *
 * @date 04 Apr 2024
 * @author Name Surname [your@email.here]
 */

#include "ucli.h"

// Aliases

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

ucli_state_t ucli_state = UCLI_STATE_INIT;

void ucli_init(void) {
    ucli_state = ucli_run_state(ucli_state, NULL);
}

void ucli_routine(uint8_t* byte) {
    if (byte != NULL)
    {
        ucli_event_data_t event = { byte };
        ucli_event_trigger(&event);
    }

    ucli_state = ucli_run_state(ucli_state, NULL);
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

void ucli_handle_backspace(void) {

}

void ucli_handle_enter(void) {

}