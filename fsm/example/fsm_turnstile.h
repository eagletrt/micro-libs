
#ifndef FSM_TURNSTILE_H
#define FSM_TURNSTILE_H

#include <inttypes.h>

#include "../fsm.h"

/**
 * @brief   Definition of all states
 */
typedef enum {
	TURNSTILE_INIT,
	TURNSTILE_WAIT,
	TURNSTILE_UNLOCK,
	TURNSTILE_ERROR,
	TURNSTILE_NUM_STATES
} turnstile_states;

extern fsm fsm_turnstile;

void fsm_turnstile_init();

#endif
