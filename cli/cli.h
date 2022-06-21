/**
 * @file		cli.h
 * @brief		This file contains the functions to create the cli
 *
 * @date		Oct 24, 2019
 *
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 * @author	Simone Ruffini [simone.ruffini@studenti.unitn.it]
 */

#ifndef CLI_H
#define CLI_H
#include "../llist/llist.h"
#include "main.h"

#include <inttypes.h>
#include <stdbool.h>

#define BUF_SIZE 64
#define PS_SIZE  3
// TODO: Enforce this limit
#define HISTORY_MAX_SIZE    100
#define CLI_TX_BUF_LEN      4096

typedef void cli_command_func_t(uint16_t argc, char **argv, char *out);
typedef enum {CLI_ESCAPE_RECEIVED, CLI_ESCAPE_CODE_WAITING, CLI_ESCAPE_NOT_RECEIVED} CLI_ESCAPE_STATE;

typedef struct buffer {
    uint8_t index;
    CLI_ESCAPE_STATE escape_state;
    char buffer[BUF_SIZE];
    char escape;
} buffer_t;

typedef struct commands {
    cli_command_func_t **functions;  // Array of pointers to command functions
    char **names;                    // Array of command names
    uint8_t count;                   // Number of commands
} commands_t;

typedef struct cli_t {
    UART_HandleTypeDef *uart;

    char input_buf;  // Input byte

    llist history;                // stream (history)
    uint32_t current_hist_index;  // Currently "selected" history item

    buffer_t current_command;

    bool receive;      // True if a byte has been received
    bool complete;     // True if the current command has been \n'd

    commands_t cmds;
} cli_t;

static const char cli_ps[PS_SIZE] = "> \0";

void cli_buf_init(buffer_t *buf);
void cli_init(cli_t *cli);
void cli_print(cli_t *cli, char *text, size_t length);
void cli_loop(cli_t *cli);
void cli_handle_interrupt(cli_t *cli);
void cli_char_receive(cli_t *cli);
uint16_t _cli_get_args(char cmd[BUF_SIZE], char *argv[BUF_SIZE]);
#endif
