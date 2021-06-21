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
//bool _fsm_set_state(fsm *FSM, uint16_t state) {
//    if (_fsm_is_transition_legal(FSM, state)) {
//        FSM->future_state = state;
//        return true;
//    }
//    return false;
//}

//int _priority(uint16_t a, uint16_t b) {
//    return a < b;
//}

/**
 * @brief	Initializes the fsm struct
 * 
 * @param	FSM		The FSM struct to init
 */
void fsm_init(fsm *FSM) {
    FSM->events      = 0;
    FSM->state_table = malloc(sizeof(struct state) * FSM->state_count);
}

void fsm_start(fsm *FSM) {
    // Enter the initial state
    if (FSM->state_table[FSM->current_state].start != NULL) {
        FSM->state_table[FSM->current_state].start(FSM);
    }
}

/**
 * @brief	Frees the allocated memory inside FSM
 *
 * @param	FSM	The fsm struct to deinit
 */
void fsm_deinit(fsm *FSM) {
    free(FSM->state_table);
}

void fsm_transition(fsm *FSM, uint16_t state) {
    if (FSM->state_table[FSM->current_state].stop != NULL) {
        FSM->state_table[FSM->current_state].stop(FSM);
    }
    FSM->current_state = state;
    if (FSM->state_table[FSM->current_state].start != NULL) {
        FSM->state_table[FSM->current_state].start(FSM);
    }
}

/**
 * @returns	The current running state
 */
uint16_t fsm_get_state(fsm *FSM) {
    return FSM->current_state;
}

void fsm_catch_event(fsm *FSM, uint32_t event) {
    FSM->events = FSM->events | 1 << event;
}

/**
 * @brief Runs the state machine
 * 
 * @param	FSM		The FSM struct
 */
void fsm_run(fsm *FSM) {
    for (uint16_t i = 0; i < 32; i++) {
        uint8_t event = FSM->events & 1 << i;
        if (event) {
            FSM->events = FSM->events & ~event;
            if (FSM->state_table[FSM->current_state].handler != NULL) {
                FSM->state_table[FSM->current_state].handler(FSM, i);
            }
        }
    }
}