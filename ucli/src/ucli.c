/**
 * @file ucli.c
 * @brief 
 *
 * @date 04 Apr 2024
 * @author Name Surname [your@email.here]
 */

#include "ucli.h"

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
