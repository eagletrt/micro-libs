/**
 * @file ucli-parser.c
 * @brief Parser for the micro CLI
 *
 * @date 08 May 2024
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#include "ucli-parser.h"

typedef enum {
    TOKEN_TYPE_UNKOWN = -1,
    TOKEN_TYPE_ID,
    TOKEN_TYPE_VERSION,
    TOKEN_TYPE_HELP,
    TOKEN_TYPE_WATCH,
    TOKEN_TYPE_COUNT
} TokenType;

typedef struct {
    TokenType type;
    size_t pos;
    uint8_t data[UCLI_PARSER_MAX_TOKEN_SIZE];
} Token;

static size_t token_index;
static Token tokens[UCLI_PARSER_MAX_TOKEN_COUNT];

void ucli_parser_routine(uint8_t byte) {

}
