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

#define TOKEN_N ((UCLI_BUFFER_LEN + 1) / 2)

typedef struct {
    char command[10];
    char args[10][10]
} parsed_command_t;

void ucli_parser_parse(char* string, parsed_command_t* cmd);

#endif // UCLI_PARSER_H
