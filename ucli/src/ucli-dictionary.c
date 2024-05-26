/**
 * @file ucli-dictionary.c
 *
 * @date May 2024
 * @author Enrico Dalla Croce (Kalsifer-742) [kalsifer742@gmail.com]
 */

#include "ucli-dictionary.h"

// === Private Includes ===

#include "ucli.h"
#include <stdint.h>
#include <string.h>

// === Private Functions ===

// MurmurHash3
// https://en.wikipedia.org/wiki/MurmurHash
static inline uint32_t __murmur_32_scramble(uint32_t k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}
uint32_t __murmur3_32(const uint8_t* key, size_t len, uint32_t seed) {
    uint32_t h = seed;
    uint32_t k;
    /* Read in groups of 4. */
    for (size_t i = len >> 2; i; i--) {
        // Here is a source of differing results across endiannesses.
        // A swap here has no effects on hash properties though.
        memcpy(&k, key, sizeof(uint32_t));
        key += sizeof(uint32_t);
        h ^= __murmur_32_scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }
    /* Read the rest. */
    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }
    // A swap is *not* necessary here because the preceding loop already
    // places the low bytes in the low places according to whatever endianness
    // we use. Swaps only apply when the memory is copied in a chunk.
    h ^= __murmur_32_scramble(k);
    /* Finalize. */
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

uint32_t _ucli_dictionary_hash(char* key) {
    size_t len = strlen(key);
    return __murmur3_32((uint8_t*)key, len, 42);
}

uint8_t _ucli_dictionary_get_bucket(char* key) {
    return _ucli_dictionary_hash(key) % BUCKETS_N;
}

// === Public Functions ===

ucli_dictionary_return_code_t ucli_dictionary_init(ucli_dictionary_t* dict) {
    for (size_t i = 0; i < BUCKETS_N; i++) {
        for (size_t j = 0; j < BUCKETS_SIZE; j++) {
            memset(dict->buckets[i][j].key, '\0', UCLI_ARGS_LEN_MAX);
            dict->buckets[i][j].function = NULL;
        }
    }

    return UCLI_DICTIONARY_RETURN_CODE_OK;
}

ucli_dictionary_return_code_t
ucli_dictionary_add(ucli_dictionary_t* dict, char* key,
                    ucli_command_function_t function) {
    if (function == NULL) {
        return UCLI_DICTIONARY_RETURN_CODE_FUN_PTR_IS_NULL;
    }

    uint8_t bucket = _ucli_dictionary_get_bucket(key);

    for (size_t i = 0; i < BUCKETS_SIZE; i++) {
        if (dict->buckets[bucket][i].function == NULL) {
            strcpy(dict->buckets[bucket][i].key, key);
            dict->buckets[bucket][i].function = function;
            return UCLI_DICTIONARY_RETURN_CODE_OK;
        }
    }

    return UCLI_DICTIONARY_RETURN_CODE_FULL;
}

ucli_dictionary_return_code_t
ucli_dictionary_get(ucli_dictionary_t* dict, char* key,
                    ucli_command_function_t* function) {
    if (function == NULL) {
        return UCLI_DICTIONARY_RETURN_CODE_FUN_PTR_IS_NULL;
    }

    uint8_t bucket = _ucli_dictionary_get_bucket(key);

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
    uint8_t bucket = _ucli_dictionary_get_bucket(key);

    for (size_t i = 0; i < BUCKETS_SIZE; i++) {
        if (strcmp(dict->buckets[bucket][i].key, key) == 0) {
            return UCLI_DICTIONARY_RETURN_CODE_OK;
        }
    }

    return UCLI_DICTIONARY_RETURN_CODE_NOT_FOUND;
}
