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

struct fsm {
	uint16_t current_state;
	uint16_t future_state;

	state_function *state_table;
};

void fsm_init(fsm *FSM, uint16_t num_states);
void fsm_run(fsm *FSM);
#endif