/**
 * @file		fsm.h
 * @brief		This file contains the primitives to setup a FSM
 *
 * @date		Oct 24, 2019
 *
 * @author		Simone Ruffini [simone.ruffini@tutanota.com]
 * @author		Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "fsm.h"

#include <stdbool.h>
#include <stdlib.h>

bool _fsm_is_transition_legal(fsm *FSM, uint16_t state) {
    return FSM->state_table[FSM->current_state][state] != NULL;
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
 * 
 * @returns whether the state has been set or not
 */
bool _fsm_set_state(fsm *FSM, uint16_t state) {
    if (_fsm_is_transition_legal(FSM, state)) {
        FSM->future_state = state;
        return true;
    }
    return false;
}

/**
 * @brief	Initializes the fsm struct
 * 
 * @param	FSM		The FSM struct to init
 */
void fsm_init(fsm *FSM, uint16_t state_count) {
    FSM->current_state = 0;  // Set initial state
    FSM->future_state  = FSM->current_state;
    FSM->state_count   = state_count;

    PQ_init(&FSM->event_queue, FSM->state_count * 2, FSM->state_count);

    FSM->state_table = malloc(sizeof(state_function) * FSM->state_count);
    for (uint16_t i = 0; i < FSM->state_count; i++) {
        FSM->state_table[i] = malloc(sizeof(state_function) * FSM->state_count);

        for (uint16_t j = 0; j < FSM->state_count; j++) {
            FSM->state_table[i][j] = NULL;
        }
    }
}

/**
 * @brief	Frees the allocated memory inside FSM
 *
 * @param	FSM	The fsm struct to deinit
 */
void fsm_deinit(fsm *FSM) {
    for (uint16_t i = 0; i < FSM->state_count; i++) {
        free(FSM->state_table[i]);
    }
    free(FSM->state_table);
}

/**
 * @returns	The current running state
 */
uint16_t fsm_get_state(fsm *FSM) {
    return FSM->current_state;
}

void fsm_handle_event(fsm *FSM, uint16_t state) {
    PQ_insert(FSM->event_queue, state, &state);
}

/**
 * @brief Runs the state machine
 * 
 * @param	FSM		The FSM struct
 */
void fsm_run(fsm *FSM) {
    if (!PQ_is_empty(FSM->event_queue)) {
        uint16_t *e = 0;
        do {
            e = (uint16_t *)PQ_peek_highest(FSM->event_queue);
            PQ_pop_highest(FSM->event_queue);

        } while (!_fsm_set_state(FSM, *e) && !PQ_is_empty(FSM->event_queue));
    }
    // rm future_state

    uint16_t future    = FSM->state_table[FSM->current_state][FSM->future_state](FSM);
    FSM->current_state = FSM->future_state;
    FSM->future_state  = future;
}