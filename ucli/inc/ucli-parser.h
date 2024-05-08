/**
 * @file ucli-parser.h
 * @brief Parser for the micro CLI
 *
 * @date 08 May 2024
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#ifndef UCLI_PARSER_H
#define UCLI_PARSER_H

#include <stdint.h>

#include "ucli.h"

#define UCLI_PARSER_MAX_TOKEN_SIZE (10U)
#define UCLI_PARSER_MAX_TOKEN_COUNT ((UCLI_BUFFER_LEN + 1U) / 2U)

/**
 * @brief Run the parser
 *
 * @param byte The input byte
 */
void ucli_parser_routine(uint8_t byte);

#endif  // UCLI_PARSER_H
