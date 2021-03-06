
#include "fsm_turnstile.h"

void main() {
	// Initialize the fsm
	fsm_turnstile_init();

	while (1) {
		// Run the state machine
		fsm_run(&fsm_turnstile);
	}
}

/**
 * @brief Interrupt callback from the card reader
 */
void cardreader_validcard_handler() { fsm_turnstile_unlock_handler(); }