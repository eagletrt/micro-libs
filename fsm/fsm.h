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

#include <inttypes.h>

typedef enum { EVENT_HANDLED, EVENT_UNKNOWN } event_result;

typedef struct fsm fsm;

typedef void (*state_function)(fsm *);
typedef event_result (*event_handler)(fsm *, uint8_t event);

struct state {
    event_handler handler;
    state_function start;
    state_function stop;
};

struct fsm {
    uint16_t current_state;

    uint32_t events;

    uint16_t state_count;
    struct state *state_table;
};

void fsm_init(fsm *FSM);
void fsm_start(fsm *FSM);
void fsm_deinit(fsm *FSM);
void fsm_transition(fsm *FSM, uint16_t state);
uint16_t fsm_get_state(fsm *FSM);
void fsm_catch_event(fsm *FSM, uint32_t event);
void fsm_run(fsm *FSM);

#endif