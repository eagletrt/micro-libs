/******************************************************************************
Finite State Machine
Project: micro-cli
Description: Simple Command Line Interface for embedded systems

Generated by gv_fsm ruby gem, see https://rubygems.org/gems/gv_fsm
gv_fsm version 0.5.0
Generation date: 2024-04-04 14:35:36 +0200
Generated from: ucli.dot
The finite state machine has:
  5 states
  7 transition functions
Functions and types have been generated with prefix "ucli_"
******************************************************************************/

#include "ucli_fsm.h"

/*** USER CODE BEGIN GLOBALS ***/
#include "ucli.h"
#include "ring-buffer.h"

RingBuffer(uint8_t, INPUT_BUFFER_LEN) input_buffer;
ucli_command_buffer_t command_buffer;

/*** USER CODE END GLOBALS ***/


// GLOBALS
// State human-readable names
const char *ucli_state_names[] = {"init", "idle", "drop", "parse", "exec"};

// List of state functions
state_func_t *const ucli_state_table[UCLI_NUM_STATES] = {
  ucli_do_init,  // in state init
  ucli_do_idle,  // in state idle
  ucli_do_drop,  // in state drop
  ucli_do_parse, // in state parse
  ucli_do_exec,  // in state exec
};

// Table of transition functions
transition_func_t *const ucli_transition_table[UCLI_NUM_STATES][UCLI_NUM_STATES] = {
  /* states:     init                , idle                , drop                , parse               , exec                 */
  /* init    */ {NULL                , ucli_init_to_idle   , NULL                , NULL                , NULL                }, 
  /* idle    */ {NULL                , NULL                , ucli_drop           , ucli_parse          , NULL                }, 
  /* drop    */ {NULL                , ucli_drop_to_idle   , NULL                , NULL                , NULL                }, 
  /* parse   */ {NULL                , ucli_parse_to_idle  , NULL                , NULL                , ucli_execute        }, 
  /* exec    */ {NULL                , ucli_execute_to_idle, NULL                , NULL                , NULL                }, 
};

// Triggered event
ucli_event_data_t * ucli_fired_event = NULL;

// Function to check if an event has fired
inline bool ucli_is_event_triggered() {
    return ucli_fired_event != NULL;
}

// Function to trigger an event
inline void ucli_event_trigger(ucli_event_data_t *event) {
    if (ucli_fired_event != NULL)
        return;
    ucli_fired_event = event ? event : &(ucli_event_data_t){};
}


/*  ____  _        _       
 * / ___|| |_ __ _| |_ ___ 
 * \___ \| __/ _` | __/ _ \
 *  ___) | || (_| | ||  __/
 * |____/ \__\__,_|\__\___|
 *                         
 *   __                  _   _                 
 *  / _|_   _ _ __   ___| |_(_) ___  _ __  ___ 
 * | |_| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
 * |  _| |_| | | | | (__| |_| | (_) | | | \__ \
 * |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
 */                                             

// Function to be executed in state init
// valid return states: UCLI_STATE_IDLE
ucli_state_t ucli_do_init(ucli_state_data_t *data) {
  ucli_state_t next_state = UCLI_STATE_IDLE;
  
  
  /*** USER CODE BEGIN DO_INIT ***/

    ring_buffer_init(&input_buffer, uint8_t, INPUT_BUFFER_LEN, NULL, NULL);
    ucli_command_buffer_init(&command_buffer);

  /*** USER CODE END DO_INIT ***/
  
  switch (next_state) {
    case UCLI_STATE_IDLE:
      break;
    default:
      next_state = UCLI_NO_CHANGE;
  }
  
  return next_state;
}


// Function to be executed in state idle
// valid return states: UCLI_NO_CHANGE, UCLI_STATE_IDLE, UCLI_STATE_DROP, UCLI_STATE_PARSE
ucli_state_t ucli_do_idle(ucli_state_data_t *data) {
  ucli_state_t next_state = UCLI_NO_CHANGE;
  
  
  /*** USER CODE BEGIN DO_IDLE ***/
  
    if (ucli_is_event_triggered()) {
        uint8_t new_byte = ucli_fired_event->byte;
        ring_buffer_push_back(&input_buffer, &new_byte);

        #if ENABLE_ECHO
            ucli_send_echo(new_byte);   
        #endif

        if (ucli_is_valid_char(new_byte))
        {
            if (!ucli_command_buffer_is_full(&command_buffer))
            {
                uint8_t new_char;
                ring_buffer_pop_back(&input_buffer, &new_char);
                ucli_command_buffer_push(&command_buffer, new_char); // the result is ignored
            } else {
                next_state = UCLI_STATE_DROP;
            }
            
        } else if (ucli_is_valid_special_char(new_byte)){ //probably error here
            switch (new_byte) {
                case SPECIAL_CHAR_BACKSPACE:
                    ucli_command_buffer_pop(&command_buffer);
                    ucli_send_backspace();
                    break;
                case SPECIAL_CHAR_LINE_FEED:
                    next_state = UCLI_STATE_PARSE;
                    break;
                
                default:
                    next_state = UCLI_STATE_DROP;
                    break;
                }
        } else {
            next_state = UCLI_STATE_DROP;
        }
    }

  /*** USER CODE END DO_IDLE ***/
  
  switch (next_state) {
    case UCLI_NO_CHANGE:
    case UCLI_STATE_IDLE:
    case UCLI_STATE_DROP:
    case UCLI_STATE_PARSE:
      break;
    default:
      next_state = UCLI_NO_CHANGE;
  }
  
  return next_state;
}


// Function to be executed in state drop
// valid return states: UCLI_NO_CHANGE, UCLI_STATE_IDLE, UCLI_STATE_DROP
ucli_state_t ucli_do_drop(ucli_state_data_t *data) {
  ucli_state_t next_state = UCLI_NO_CHANGE;
  
  
  /*** USER CODE BEGIN DO_DROP ***/

    if (ucli_is_event_triggered()) {
        uint8_t new_byte = ucli_fired_event->byte;
        ring_buffer_push_back(&input_buffer, &new_byte);

        if (ucli_is_valid_special_char(new_byte))
        {
            switch (new_byte) {
                case SPECIAL_CHAR_LINE_FEED:
                    next_state = UCLI_STATE_IDLE;
                    break;
                
                default:
                    next_state = UCLI_STATE_DROP;
                    break;
            }
        }
    }
    
  
  /*** USER CODE END DO_DROP ***/
  
  switch (next_state) {
    case UCLI_NO_CHANGE:
    case UCLI_STATE_IDLE:
    case UCLI_STATE_DROP:
      break;
    default:
      next_state = UCLI_NO_CHANGE;
  }
  
  return next_state;
}


// Function to be executed in state parse
// valid return states: UCLI_STATE_IDLE, UCLI_STATE_EXEC
ucli_state_t ucli_do_parse(ucli_state_data_t *data) {
  ucli_state_t next_state = UCLI_STATE_IDLE;
  
  
  /*** USER CODE BEGIN DO_PARSE ***/
  
  /*** USER CODE END DO_PARSE ***/
  
  switch (next_state) {
    case UCLI_STATE_IDLE:
    case UCLI_STATE_EXEC:
      break;
    default:
      next_state = UCLI_NO_CHANGE;
  }
  
  return next_state;
}


// Function to be executed in state exec
// valid return states: UCLI_NO_CHANGE, UCLI_STATE_IDLE, UCLI_STATE_EXEC
ucli_state_t ucli_do_exec(ucli_state_data_t *data) {
  ucli_state_t next_state = UCLI_NO_CHANGE;
  
  
  /*** USER CODE BEGIN DO_EXEC ***/
  
  /*** USER CODE END DO_EXEC ***/
  
  switch (next_state) {
    case UCLI_NO_CHANGE:
    case UCLI_STATE_IDLE:
    case UCLI_STATE_EXEC:
      break;
    default:
      next_state = UCLI_NO_CHANGE;
  }
  
  return next_state;
}


/*  _____                    _ _   _              
 * |_   _| __ __ _ _ __  ___(_) |_(_) ___  _ __   
 *   | || '__/ _` | '_ \/ __| | __| |/ _ \| '_ \
 *   | || | | (_| | | | \__ \ | |_| | (_) | | | | 
 *   |_||_|  \__,_|_| |_|___/_|\__|_|\___/|_| |_| 
 *                                                
 *   __                  _   _                 
 *  / _|_   _ _ __   ___| |_(_) ___  _ __  ___ 
 * | |_| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
 * |  _| |_| | | | | (__| |_| | (_) | | | \__ \
 * |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
 */    
                                         
// This function is called in 1 transition:
// 1. from init to idle
void ucli_init_to_idle(ucli_state_data_t *data) {
  
  /*** USER CODE BEGIN INIT_TO_IDLE ***/
  
  /*** USER CODE END INIT_TO_IDLE ***/
}

// This function is called in 1 transition:
// 1. from idle to drop
void ucli_drop(ucli_state_data_t *data) {
  
  /*** USER CODE BEGIN DROP ***/
  
    ucli_command_buffer_clean(&command_buffer);

  /*** USER CODE END DROP ***/
}

// This function is called in 1 transition:
// 1. from idle to parse
void ucli_parse(ucli_state_data_t *data) {
  
  /*** USER CODE BEGIN PARSE ***/
  
  /*** USER CODE END PARSE ***/
}

// This function is called in 1 transition:
// 1. from drop to idle
void ucli_drop_to_idle(ucli_state_data_t *data) {
  
  /*** USER CODE BEGIN DROP_TO_IDLE ***/
  
  /*** USER CODE END DROP_TO_IDLE ***/
}

// This function is called in 1 transition:
// 1. from parse to idle
void ucli_parse_to_idle(ucli_state_data_t *data) {
  
  /*** USER CODE BEGIN PARSE_TO_IDLE ***/
  
  /*** USER CODE END PARSE_TO_IDLE ***/
}

// This function is called in 1 transition:
// 1. from parse to exec
void ucli_execute(ucli_state_data_t *data) {
  
  /*** USER CODE BEGIN EXECUTE ***/
  
  /*** USER CODE END EXECUTE ***/
}

// This function is called in 1 transition:
// 1. from exec to idle
void ucli_execute_to_idle(ucli_state_data_t *data) {
  
  /*** USER CODE BEGIN EXECUTE_TO_IDLE ***/
  
  /*** USER CODE END EXECUTE_TO_IDLE ***/
}


/*  ____  _        _        
 * / ___|| |_ __ _| |_ ___  
 * \___ \| __/ _` | __/ _ \
 *  ___) | || (_| | ||  __/ 
 * |____/ \__\__,_|\__\___| 
 *                          
 *                                              
 *  _ __ ___   __ _ _ __   __ _  __ _  ___ _ __ 
 * | '_ ` _ \ / _` | '_ \ / _` |/ _` |/ _ \ '__|
 * | | | | | | (_| | | | | (_| | (_| |  __/ |   
 * |_| |_| |_|\__,_|_| |_|\__,_|\__, |\___|_|   
 *                              |___/           
 */

ucli_state_t ucli_run_state(ucli_state_t cur_state, ucli_state_data_t *data) {
  ucli_event_data_t *prev_ev = ucli_fired_event;
  ucli_state_t new_state = ucli_state_table[cur_state](data);
  // Reset event status
  if (prev_ev != NULL)
    ucli_fired_event = NULL;
  if (new_state == UCLI_NO_CHANGE) new_state = cur_state;
  transition_func_t *transition = ucli_transition_table[cur_state][new_state];
  if (transition)
    transition(data);
  return new_state;
};

#ifdef TEST_MAIN
#include <unistd.h>
int main() {
  ucli_state_t cur_state = UCLI_STATE_INIT;
  do {
    cur_state = ucli_run_state(cur_state, NULL);
    sleep(1);
  } while (1);
  return 0;
}
#endif
