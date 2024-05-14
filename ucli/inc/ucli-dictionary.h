#ifndef UCLI_DICTIONARY_H
#define UCLI_DICTIONARY_H

#include "ucli.h"
#include <stdint.h>

// === Parameters ===

#define BUCKETS_N 10
#define BUCKETS_SIZE 3

// === Public Types ===

typedef struct {
    char* key;
    ucli_command_function_t function;
} ucli_dictionary_key_value_pair_t;

typedef struct {
    ucli_dictionary_key_value_pair_t buckets[BUCKETS_N][BUCKETS_SIZE];
} ucli_dictionary_t;

typedef enum {
    UCLI_DICTIONARY_RETURN_CODE_OK,
    UCLI_DICTIONARY_RETURN_CODE_ERROR,
    UCLI_DICTIONARY_RETURN_CODE_FULL,
    UCLI_DICTIONARY_RETURN_CODE_NOT_FOUND,
} ucli_dictionary_return_code_t;

// === Public Functions ===

void ucli_dictionary_init(ucli_dictionary_t* dict);
ucli_dictionary_return_code_t
ucli_dictionary_add(ucli_dictionary_t* dict, char* key,
                    ucli_command_function_t function);
ucli_dictionary_return_code_t
ucli_dictionary_get(ucli_dictionary_t* dict, char* key,
                    ucli_command_function_t function);
ucli_dictionary_return_code_t
ucli_dictionary_contains_key(ucli_dictionary_t* dict, char* key);

#endif // UCLI_DICTIONARY_H