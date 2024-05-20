/**
 * @file ucli-parser.c
 * @brief Parser for the micro CLI
 *
 * @date 08 May 2024
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#include "ucli-parser.h"

// === Private Includes ===

#include "ucli-private.h"
#include <stdint.h>
#include <string.h>

typedef enum { TOKEN_COMMAND, TOKEN_ARGUMENT, TOKEN_UNKNOWN } token_type_t;

typedef struct {
    token_type_t token_type;
    char value[10];
} token_t;

void ucli_parser_lexer(char* string, token_t* tokens) {
    char* delim = " ";
    char* token;

    token = strtok(string, delim);

    uint8_t i = 0;
    while (token != NULL) {
        if (ucli_dictionary_contains_key(&commands, token) ==
            UCLI_DICTIONARY_RETURN_CODE_OK) {
            tokens[i].token_type = TOKEN_COMMAND;
            strncpy(tokens[i].value, token, 10);
        } else {
            tokens[i].token_type = TOKEN_ARGUMENT;
            strncpy(tokens[i].value, token, 10);
        }

        token = strtok(NULL, delim);
        i++;
    }
}

void ucli_parser_parse(char* string, parsed_command_t* cmd) {
    token_t tokens[TOKEN_N];
    for (size_t i = 0; i < TOKEN_N; i++) {
        tokens[i].token_type = TOKEN_UNKNOWN;
        memset(tokens[i].value, '\0', 10);
    }

    ucli_parser_lexer(string, tokens);

    if (tokens[0].token_type != TOKEN_COMMAND) {
        // TO-DO: Error
    }

    strcpy(cmd->command, tokens[0].value);

    int j = 0;
    for (size_t i = 1; i < TOKEN_N; i++) {
        if (tokens[i].token_type == TOKEN_ARGUMENT) {
            strcpy(cmd->args[j], tokens[i].value);
            j++;
        }
    }
    cmd->argc = j;
}
