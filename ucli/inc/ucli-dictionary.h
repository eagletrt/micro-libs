/**
 * @file ucli-dictionary.h
 *
 * @date May 2024
 * @author Enrico Dalla Croce (Kalsifer-742) [kalsifer742@gmail.com]
 */

#ifndef UCLI_DICTIONARY_H
#define UCLI_DICTIONARY_H

// == Private Includes ==

#include "ucli.h"

// === Parameters ===

#define BUCKETS_N 15
#define BUCKETS_SIZE 5

// === Private Types ===

typedef struct {
    char key[UCLI_ARGS_LEN_MAX];
    ucli_command_function_t function;
} _ucli_dictionary_key_value_pair_t;

// === Public Types ===

typedef struct {
    _ucli_dictionary_key_value_pair_t buckets[BUCKETS_N][BUCKETS_SIZE];
} ucli_dictionary_t;

typedef enum {
    UCLI_DICTIONARY_RETURN_CODE_OK,
    UCLI_DICTIONARY_RETURN_CODE_ERROR,
    UCLI_DICTIONARY_RETURN_CODE_FULL,
    UCLI_DICTIONARY_RETURN_CODE_NOT_FOUND,
} ucli_dictionary_return_code_t;

// === Public Functions ===

ucli_dictionary_return_code_t ucli_dictionary_init(ucli_dictionary_t* dict);
ucli_dictionary_return_code_t
ucli_dictionary_add(ucli_dictionary_t* dict, char* key,
                    ucli_command_function_t function);
ucli_dictionary_return_code_t
ucli_dictionary_get(ucli_dictionary_t* dict, char* key,
                    ucli_command_function_t* function);
ucli_dictionary_return_code_t
ucli_dictionary_contains_key(ucli_dictionary_t* dict, char* key);

#endif // UCLI_DICTIONARY_H