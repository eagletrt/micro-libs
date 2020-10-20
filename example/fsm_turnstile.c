#include "fsm_turnstile.h"

uint16_t do_init(fsm *FSM);
uint16_t do_wait(fsm *FSM);
uint16_t do_unlock(fsm *FSM);
uint16_t do_error(fsm *FSM);

fsm fsm_turnstile;	// Define the turnstile fsm struct. This is accessible from the outside

void fsm_turnstile_init() {
	fsm_init(&fsm_turnstile);

	// Make sure to assign EVERY slot of the array.
	fsm_turnstile.state_table[TURNSTILE_INIT] = &do_init;
	fsm_turnstile.state_table[TURNSTILE_WAIT] = &do_wait;
	fsm_turnstile.state_table[TURNSTILE_UNLOCK] = &do_unlock;
	fsm_turnstile.state_table[TURNSTILE_ERROR] = &do_error;
}

/**
 * Initialize peripherals and data structures
 */
uint16_t do_init(fsm *FSM) {
	// display_init();
	// cardreader_init();

	return TURNSTILE_WAIT;
}

/**
 * Wait for a card to be read
 */
uint16_t do_wait(fsm *FSM) {
	if (/*cardreader_readcard()*/) {
		if (/*cardreader_cardvalid()*/)
			return TURNSTILE_UNLOCK;

		return TURNSTILE_ERROR;
	}

	return TURNSTILE_WAIT;
}

/**
 * Unlock turnstile
 */
uint16_t do_unlock(fsm *FSM) {
	// display_showmessage("Card Valid! :)");
	// turnstile_unlock();
	// wait(3000);
	// turnstile_lock();

	return TURNSTILE_WAIT;
}

/**
 * The card is not valid
 */
uint16_t do_error(fsm *FSM) {
	// display_showmessage("Card Not Valid! :(");
	// wait(1000);

	return TURNSTILE_WAIT;
}