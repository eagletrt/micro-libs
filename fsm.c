/**
 * @file		fsm.h
 * @brief		This file contains the primitives to setup a FSM
 *
 * @date		Oct 24, 2019
 *
 * @author		Simone Ruffini[simone.ruffini@tutanota.com]
 * @author		Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "fsm.h"

#include <stdlib.h>

/**
 * @brief	Initializes the fsm struct
 * 
 * @param	FSM		The FSM struct to init
 */
void fsm_init(fsm *FSM, uint16_t num_states) {
	FSM->current_state = 0;	 // Set initial state
	FSM->future_state = FSM->current_state;

	FSM->state_table = malloc(sizeof(state_function) * num_states);
}

/**
 * @brief	Frees the allocated memory inside FSM
 *
 * @param	FSM	The fsm struct to deinit
 */
void fsm_deinit(fsm *FSM) {
	free(FSM->state_table);
}

/**
 * @returns	The current running state
 */
uint16_t fsm_get_state(fsm *FSM) {
	return FSM->future_state;
}

/**
 * @brief	Sets the next state to be run
 * @details	This funciton is meant to be used to trigger the fsm externally.
 * 			For example, if a critical error is detected in an interrupt callback, and immediate action is required,
 * 			this function can be called to force execution of an error state. This will tell the fsm to run the provided state next.
 * 			WARNING: Always double-check when using this function. It can lead to undefined behaviour.
 * 
 * @param	FSM		The FSM struct
 * @param	state	The next state to run
 */
void fsm_set_state(fsm *FSM, uint16_t state) {
	FSM->future_state = state;
}

/**
 * @brief Runs the state machine
 * 
 * @param	FSM		The FSM struct
 */
void fsm_run(fsm *FSM) {
	// Advance to next state
	FSM->current_state = FSM->future_state;

	// run state and save return value in future_state
	FSM->future_state = FSM->state_table[FSM->current_state](FSM);
}