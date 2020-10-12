/**
 * @file		fsm.h
 * @brief		This file contains the primitives to setup a FSM
 *
 * @date		Oct 24, 2019
 *
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 * @author	Simone Ruffini[simone.ruffini@tutanota.com]
 */

#ifndef FSM_H
#define FSM_H

#include <inttypes.h>

typedef struct fsm fsm;

typedef uint16_t (*state_function)(fsm *);

// the array pointed by states must not contain null pointers
struct fsm {
	//state_function current_state;
	//state_function future_state;
	uint16_t current_state;
	uint16_t future_state;
	uint16_t number_states;
	state_function *state_table;
	char **state_names;
};

void fsm_init(fsm *FSM);
void fsm_run_state(fsm *FSM);
#endif