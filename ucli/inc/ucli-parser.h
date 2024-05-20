/**
 * @file ucli-parser.h
 * @brief Parser for the micro CLI
 *
 * @date 08 May 2024
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#ifndef UCLI_PARSER_H
#define UCLI_PARSER_H

// === Private Includes ===

#include "ucli.h"

// === Private Defines ===

#define TOKEN_N ((UCLI_BUFFER_LEN + 1) / 2)

typedef enum {
    UCLI_PARSER_RETURN_CODE_OK,
    UCLI_PARSER_RETURN_CODE_UNKNOWN_COMMAND,
} ucli_parser_return_code_t;

typedef struct {
    char command[UCLI_ARGS_N_MAX];
    int argc;
    char args[UCLI_ARGS_LEN_MAX][UCLI_ARGS_N_MAX];
} parsed_command_t;

// === Public Functions ===

ucli_parser_return_code_t ucli_parser_parse(char* string, parsed_command_t* cmd);

#endif // UCLI_PARSER_H
