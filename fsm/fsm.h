/**
 * @file		fsm.h
 * @brief		This file contains the primitives to setup a FSM
 *
 * @date		Oct 24, 2019
 *
 * @author		Simone Ruffini [simone.ruffini@tutanota.com]
 * @author		Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#ifndef FSM_H
#define FSM_H

#include "priority_queue.h"

#include <inttypes.h>

typedef struct fsm fsm;

typedef uint16_t (*state_function)(fsm *);

struct fsm {
    uint16_t current_state;
    uint16_t future_state;
    uint16_t state_count;

    PQ_QueueTypeDef event_queue;
    state_function **state_table;
};

void fsm_init(fsm *FSM, uint16_t state_count);
void fsm_deinit(fsm *FSM);
uint16_t fsm_get_state(fsm *FSM);
void fsm_handle_event(fsm *FSM, uint16_t state);
void fsm_run(fsm *FSM);

#endif