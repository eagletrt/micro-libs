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

#include <string.h>

struct fsm {
    uint32_t current_state;

    size_t event_count;
    bool *events_sync;
    bool *events_async;

    size_t state_count;
    fsm_state *state_table;
};

fsm fsm_init(size_t state_count, size_t event_count) {
    fsm handle = (fsm)malloc(sizeof(struct fsm));

    (handle)->current_state = 0;
    (handle)->state_count   = state_count;
    (handle)->event_count   = event_count;

    (handle)->events_async = malloc(sizeof(bool) * (handle)->event_count);
    (handle)->events_sync  = malloc(sizeof(bool) * (handle)->event_count);
    (handle)->state_table  = malloc(sizeof(struct state) * (handle)->state_count);

    for (size_t i = 0; i < (handle)->event_count; i++) {
        (handle)->events_async[i] = false;
        (handle)->events_sync[i]  = false;
    }

    return handle;
}

void fsm_deinit(fsm *handle) {
    free((*handle)->state_table);
    free((*handle)->events_async);
    free((*handle)->events_sync);
    free(*handle);
}

void fsm_start(fsm handle) {
    // Enter the initial state
    if (handle->state_table[handle->current_state].entry != NULL) {
        handle->state_table[handle->current_state].entry(handle);
    }
}

void fsm_set_state(fsm handle, uint32_t id, fsm_state *state) {
    memcpy(&(handle->state_table[id]), state, sizeof(fsm_state));
}

void fsm_transition(fsm handle, uint32_t state) {
    if (handle->state_table[handle->current_state].exit != NULL) {
        handle->state_table[handle->current_state].exit(handle);
    }
    handle->current_state = state;
    if (handle->state_table[handle->current_state].entry != NULL) {
        handle->state_table[handle->current_state].entry(handle);
    }
}

uint32_t fsm_get_state(fsm handle) {
    return handle->current_state;
}

void fsm_catch_event(fsm handle, uint32_t event) {
    if (handle->events_async[event] == handle->events_sync[event]) {
        handle->events_async[event] = !handle->events_sync[event];
    }
}

void fsm_run(fsm handle) {
    for (size_t i = 0; i < handle->event_count; i++) {
        if (handle->events_async[i] != handle->events_sync[i] &&
            handle->state_table[handle->current_state].handler != NULL) {
            if (handle->state_table[handle->current_state].handler(handle, i) == EVENT_HANDLED) {
                handle->events_sync[i] = handle->events_async[i];
            }
        }
    }
}
