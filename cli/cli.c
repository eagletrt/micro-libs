/**
 * @file		cli.c
 * @brief		This file contains the functions to create the cli
 *
 * @date		Oct 24, 2019
 *
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 * @author	Simone Ruffini [simone.ruffini@studenti.unitn.it]
 */

#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HIST_UP   1
#define HIST_DOWN -1

void cli_buf_init(buffer_t *buf) {
    buf->index     = 0;
    buf->buffer[0] = '\0';
}

void cli_init(cli_t *cli) {
    cli_buf_init(&cli->current_command);

    cli->history = llist_init(NULL, NULL);

    //the history always has a top void buffer (use case: when going down history this buffer will be used
    //for ther partial command written before going into history)
    buffer_t *first_void_buffer = (buffer_t *)malloc(sizeof(buffer_t));
    cli_buf_init(first_void_buffer);

    llist_push(cli->history, (llist_node)first_void_buffer);

    cli->complete           = false;
    cli->receive            = true;
    cli->escaping           = BUF_SIZE;
    cli->current_hist_index = 0;
}

uint16_t cli_clean(char *cmd) {
    uint16_t cursor = 0;

    for (uint16_t i = 0; i < BUF_SIZE; i++) {
        if (cmd[i] == '\b') {
            // Check backspace
            if (cursor > 0) {
                cursor--;
                cmd[cursor] = '\0';
            }
        } else if (cmd[i] == '\033') {
            cmd[cursor] = '\0';
            return cursor;
        } else {
            // Add to buffer
            cmd[cursor] = cmd[i];

            if (cmd[i] == '\0') {
                // Exit if done
                return cursor;
            }
            cursor++;
        }
    }
    return cursor;
}

/**
 * @brief Splits a given command into arguments
 * 
 * @param cmd	The input command
 * @param argv	The output arg array
 * 
 * @returns the number of args
 */
uint16_t _cli_get_args(char cmd[BUF_SIZE], char *argv[BUF_SIZE]) {
    uint16_t argc = 0;
    while(*cmd == ' ') ++cmd; //front trim

    if(strlen(cmd) == 0){
        strcpy(cmd, "?"); //show help on empty cmd
    }
    argv[argc++]    = cmd;

    for (uint16_t i = 0; cmd[i] != '\0'; i++) {
        if (cmd[i] == ' ') {
            cmd[i]       = '\0';
            argv[argc++] = cmd + (i + 1);
        }
    }
    return argc;
}

void cli_print(cli_t *cli, char *text, size_t length) {
    while (!(cli->uart->gState == HAL_UART_STATE_READY))
        ;

    // We don't use interrupt transmit because this fn can get called from a callback
    HAL_UART_Transmit(cli->uart, (uint8_t *)text, length, 500);
}

bool _cli_get_next_history_node(cli_t *cli, int8_t direction, llist_node *next) {
    // If overflow on current_hist_index no problem, next becomes NULL
    return llist_get(cli->history, cli->current_hist_index + direction, next) == LLIST_SUCCESS;
}

/**
 * @brief Handles history
 */
void cli_handle_escape(cli_t *cli) {
    if (cli->current_command.buffer[cli->current_command.index - 1] == '[') {
        cli->escaping = BUF_SIZE;
        int8_t direction;

        if (cli->current_command.buffer[cli->current_command.index] == 'A') {  // UP
            direction = HIST_UP;
        } else if (cli->current_command.buffer[cli->current_command.index] == 'B') {  // DOWN
            direction = HIST_DOWN;
        } else {
            // Unknown escape sequence
            cli->current_command.index -= 2;
            return;
        }

        buffer_t *hist;
        if (!_cli_get_next_history_node(cli, direction, (llist_node)&hist)) {
            // No history
            cli->current_command.index -= 2;
            return;
        }

        cli->current_hist_index += direction;

        char new_buffer[BUF_SIZE] = {'\0'};
        strcat(new_buffer, "\r");  // CR
        if (hist->index < cli->current_command.index - 2) {
            // If the current command is longer than the history's we need to clear the screen
            sprintf(new_buffer + 1, "%-*c\r", cli->current_command.index, (uint8_t)' ');
        }
        strcat(new_buffer, cli_ps);

        // TODO: Handle parameters separated by \0
        strncat(new_buffer, hist->buffer, hist->index);

        memcpy(&(cli->current_command), hist, sizeof(buffer_t));

        HAL_UART_Transmit(cli->uart, (uint8_t *)new_buffer, strlen(new_buffer), 100); //doing this in IT mode sbora su tut
    } else {
        // Unknown escape sequence
        cli->escaping = BUF_SIZE;
        cli->current_command.index -= 2;
        return;
    }
}

/**
 * @brief Main CLI loop
 * @details When called, this function checks the current cli status and calls the command parser when needed.
 * 			Call this in your main loop for best responsiveness.
 */
void cli_loop(cli_t *cli) {
    if (cli->receive) {
        cli->receive = false;
        cli_char_receive(cli);
        HAL_UART_Receive_IT(cli->uart, (uint8_t *)&cli->input_buf, 1);
    }

    if (cli->complete) {
        cli->complete = false;

        if (cli->escaping < BUF_SIZE) {
            cli_handle_escape(cli);
            return;
        }

        // Clean the buffer from backspaces
        cli->current_command.index = cli_clean(cli->current_command.buffer);

        {  // Save in history
            buffer_t *last_buf = NULL;
            llist_get(cli->history, 1, (llist_node *)&last_buf);
            // Check if last history entry is equal to the current
            bool comp = false;
            if ((llist_get(cli->history, 1, (llist_node *)&last_buf) == LLIST_NODE_NOT_FOUND) ||
                (strcmp(cli->current_command.buffer, last_buf->buffer) != 0)) {
                comp = true;
            }

            if (cli->current_command.index > 0 && comp) {
                // If the last command wasn't empty, we save it to history.

                //the command is not directly added to the history but instead the first node on top
                //is modified since this one is always empty (the purpose of this node is descirbed in cli_init)
                memcpy(llist_get_head(cli->history), &cli->current_command, sizeof(buffer_t));

                //after the node has been changed a new void node has to be placed on top of the history
                buffer_t *top_history_virgin_buffer = (buffer_t *)malloc(sizeof(buffer_t));
                cli_buf_init(top_history_virgin_buffer);

                llist_push(cli->history, (llist_node)top_history_virgin_buffer);
            }
        }

        char *argv[BUF_SIZE] = {'\0'};
        uint16_t argc        = _cli_get_args(cli->current_command.buffer, argv);

        // TODO: Make this better
        char tx_buf[4000] = "?\r\n";

        // Check which command corresponds with the buffer
        for (uint16_t i = 0; i < cli->cmds.count; i++) {
            //size_t len = strlen(cli->cmds.names[i]);

            if (strcmp(argv[0], cli->cmds.names[i]) == 0) {
                cli->cmds.functions[i](argc, argv, tx_buf);
                break;
            }
        }
        // Prepare current_command for the next command
        cli_buf_init(&cli->current_command);

        cli->current_hist_index = 0;
        //use if no void history node is used
        //cli->current_hist_index = NULL;

        strcat(tx_buf, cli_ps);
        HAL_UART_Transmit_IT(cli->uart, (uint8_t *)tx_buf, strlen(tx_buf));
    }
}

void cli_handle_interrupt(cli_t *cli) {
    cli->receive = true;
}

/**
 * @brief Interrupt callback
 * @details Call this function when a char is received
 */
void cli_char_receive(cli_t *cli) {
    if (cli->input_buf != '\0') {
        if (cli->input_buf == '\033') {  // Arrow
            cli->escaping = cli->current_command.index;
        } else if (cli->input_buf == '\r' || cli->input_buf == '\n') {
            cli->complete                                           = true;
            cli->current_command.buffer[cli->current_command.index] = '\0';

            HAL_UART_Transmit(cli->uart, (uint8_t *)"\r\n", 2, 10);

            return;
        } else if(cli->input_buf == '\b'){
            if(cli->current_command.index > 0){
                HAL_UART_Transmit(cli->uart, (uint8_t*)"\b \b", 3, 10);
                cli->current_command.index--;
            }
            return;
        }

        cli->current_command.buffer[cli->current_command.index] = cli->input_buf;

        if (cli->escaping < BUF_SIZE && cli->current_command.index > cli->escaping + 1) {
            cli->complete = true;
            return;
        }
        cli->current_command.index++;

        // Echo the last char
        // TODO: CHeck for echo
        //if (cli->escaping == BUF_SIZE && CLI_ECHO) {
        if (cli->escaping == BUF_SIZE) {
            HAL_UART_Transmit(cli->uart, (uint8_t *)&cli->input_buf, 1, 10);
        }

        if (cli->current_command.index == BUF_SIZE) {
            cli->complete = true;
        }
    }
}
