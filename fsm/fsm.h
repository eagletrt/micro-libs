/**
 * @file        fsm.h
 * @brief		Finite state machine implementation in C
 *
 * @date		Oct 24, 2019
 *
 * @author		Simone Ruffini [simone.ruffini@tutanota.com]
 * @author		Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#ifndef FSM_H
#define FSM_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct fsm *fsm;

typedef void (*state_function)(fsm);
typedef void (*event_handler)(fsm, uint8_t event);

typedef struct state {
    event_handler handler;
    state_function entry;
    state_function run;
    state_function exit;
} fsm_state;

/**
 * @brief Initializes a FSM instance
 * 
 * @param state_count Number of states
 * @param event_count Number of events
 * @param run_callback Callback function that is called after each state is run
 * @param transition_callback Callback function that is called after each state transition
 * @return fsm The initialized FSM handle
 */
fsm fsm_init(size_t state_count, size_t event_count, state_function run_callback, state_function transition_callback);

/**
 * @brief Destroys the FSM
 * 
 * @param handle The FSM instance to deinit
 */
void fsm_deinit(fsm *handle);

/**
 * @brief Starts the FSM by running the entry function of the initial state
 * 
 * @param handle The FSM instance handle
 */
void fsm_start(fsm handle);

/**
 * @brief Adds a state to the internal state table
 * 
 * @param handle The FSM instance handle
 * @param id ID of the state
 * @param state FSM state definition
 */
void fsm_set_state(fsm handle, uint32_t id, fsm_state *state);

/**
 * @brief Executes a state transition
 * @details Runs the exit state of the current state, and runs the entry state of the next state. If present the transition callback is executed
 * @note Use this on event handlers. Avoid using it outside of the FSM. Triggering events is the reccomended action for that use case.
 * 
 * @param handle The FSM instance handle
 * @param state The next state
 */
void fsm_transition(fsm handle, uint32_t state);

/**
 * @brief Returns the current running state
 * 
 * @param handle The FSM instance handle
 * @return The current state
 */
uint32_t fsm_get_state(fsm handle);

/**
 * @brief Sets the internal register for a given event
 * 
 * @param handle The FSM instance handle
 * @param event Event to set
 */
void fsm_trigger_event(fsm handle, uint32_t event);

/**
 * @brief Runs the FSM event handlers
 * 
 * @param handle The FSM instance handle
 */
void fsm_run(fsm handle);

#endif