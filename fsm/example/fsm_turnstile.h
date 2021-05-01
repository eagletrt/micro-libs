
#ifndef FSM_TURNSTILE_H
#define FSM_TURNSTILE_H

#include <inttypes.h>

#include "../fsm.h"

/**
 * @brief   Definition of all states
 */
typedef enum {
	TURNSTILE_INIT,
	TURNSTILE_IDLE,
	TURNSTILE_UNLOCK,
	TURNSTILE_WAIT_LOCK,
	TURNSTILE_ERROR,
	TURNSTILE_NUM_STATES
} turnstile_states;

extern fsm fsm_turnstile;

void fsm_turnstile_init();

/**
 * @brief	Handler for the unlock event
 * @details	This function gets called when a valid card has been swiped on the
 * reader. It tells the state machine to unlock the turnstile
 */
void fsm_turnstile_unlock_handler();

#endif
