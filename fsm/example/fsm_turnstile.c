#include "fsm_turnstile.h"

uint16_t _do_init(fsm *FSM);
uint16_t _do_idle(fsm *FSM);
uint16_t _do_unlock(fsm *FSM);
uint16_t _do_wait_lock(fsm *FSM);
uint16_t _do_error(fsm *FSM);

fsm fsm_turnstile;	// Define the turnstile fsm struct. This is accessible from
					// the outside

uint32_t unlock_time = 0;

void fsm_turnstile_init() {
	fsm_init(&fsm_turnstile, TURNSTILE_NUM_STATES);

	// Make sure to assign EVERY slot of the array.
	fsm_turnstile.state_table[TURNSTILE_INIT] = &_do_init;
	fsm_turnstile.state_table[TURNSTILE_IDLE] = &_do_idle;
	fsm_turnstile.state_table[TURNSTILE_UNLOCK] = &_do_unlock;
	fsm_turnstile.state_table[TURNSTILE_WAIT_LOCK] = &_do_wait_lock;
	fsm_turnstile.state_table[TURNSTILE_ERROR] = &_do_error;
}

void fsm_turnstile_unlock_handler() {
	// Here we check if the fsm is in a state which is compatible with the
	// unlock event and we set the future state accordingly
	switch (fsm_get_state(&fsm_turnstile)) {
		case TURNSTILE_IDLE:
		case TURNSTILE_WAIT_LOCK:
			fsm_set_state(&fsm_turnstile, TURNSTILE_UNLOCK);
			break;
		default:
			// current state not compatible
			break;
	}
}

/**
 * @brief Initialize peripherals and data structures
 */
uint16_t _do_init(fsm *FSM) {
	// display_init();
	// cardreader_init();

	return TURNSTILE_IDLE;
}

/**
 * @brief Wait for a card to be read
 */
uint16_t _do_idle(fsm *FSM) {
	// display_showmessage(time.now());
	return TURNSTILE_IDLE;
}

/**
 * @brief Unlock turnstile
 */
uint16_t _do_unlock(fsm *FSM) {
	// display_showmessage("Card Valid! :)");
	// turnstile_unlock();
	// unlock_time = time.now();

	return TURNSTILE_WAIT_LOCK;
}

uint16_t _do_wait_lock(fsm *FSM) {
	if (/* time.now() - unlock_time >= 3000 */) {
		// turnstile_lock();
		return TURNSTILE_IDLE;
	}

	return TURNSTILE_WAIT_LOCK;
}

/**
 * @brief The card is not valid
 */
uint16_t _do_error(fsm *FSM) {
	// display_showmessage("Card Not Valid! :(");
	// wait(2000);

	return TURNSTILE_IDLE;
}