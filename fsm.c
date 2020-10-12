/**
 * @file		fsm.h
 * @brief		This file contains the primitives to setup a FSM
 *
 * @date		Oct 24, 2019
 *
 * @author	Simone Ruffini[simone.ruffini@tutanota.com]
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "fsm.h"

void fsm_init(fsm *FSM) {
	FSM->current_state = 0;	 //FSM->state_table[0];
	FSM->future_state = FSM->current_state;
}

void fsm_run_state(fsm *FSM) {
	FSM->current_state = FSM->future_state;
	FSM->future_state = FSM->state_table[FSM->current_state](FSM);
}