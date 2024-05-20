#include "ucli-dictionary.h"

// === Private Includes ===

#include <stddef.h>
#include <stdint.h>
#include <string.h>

uint32_t ucli_dictionary_hash(char* str) {
    uint32_t hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

uint8_t ucli_dictionary_get_bucket(char* key) {
    return ucli_dictionary_hash(key) % BUCKETS_N;
}

void ucli_dictionary_init(ucli_dictionary_t* dict) {
    for (size_t i = 0; i < BUCKETS_N; i++) {
        for (size_t j = 0; j < BUCKETS_SIZE; j++) {
            dict->buckets[i][j].key = "/0";
            dict->buckets[i][j].function = NULL;
        }
    }
}

ucli_dictionary_return_code_t
ucli_dictionary_add(ucli_dictionary_t* dict, char* key,
                    ucli_command_function_t function) {
    uint8_t bucket = ucli_dictionary_get_bucket(key);

    for (size_t i = 0; i < BUCKETS_SIZE; i++) {
        if (dict->buckets[bucket][i].function == NULL) {
            dict->buckets[bucket][i].key = key;
            dict->buckets[bucket][i].function = function;
            return UCLI_DICTIONARY_RETURN_CODE_OK;
        }
    }

    return UCLI_DICTIONARY_RETURN_CODE_FULL;
}

ucli_dictionary_return_code_t
ucli_dictionary_get(ucli_dictionary_t* dict, char* key,
                    ucli_command_function_t* function) {
    uint8_t bucket = ucli_dictionary_get_bucket(key);

    for (size_t i = 0; i < BUCKETS_SIZE; i++) {
        if (strcmp(dict->buckets[bucket][i].key, key) == 0) {
            *function = dict->buckets[bucket][i].function;
            return UCLI_DICTIONARY_RETURN_CODE_OK;
        }
    }

    return UCLI_DICTIONARY_RETURN_CODE_NOT_FOUND;
}

ucli_dictionary_return_code_t
ucli_dictionary_contains_key(ucli_dictionary_t* dict, char* key) {
    uint8_t bucket = ucli_dictionary_get_bucket(key);

    for (size_t i = 0; i < BUCKETS_SIZE; i++) {
        if (strcmp(dict->buckets[bucket][i].key, key) == 0) {
            return UCLI_DICTIONARY_RETURN_CODE_OK;
        }
    }

    return UCLI_DICTIONARY_RETURN_CODE_NOT_FOUND;
}
