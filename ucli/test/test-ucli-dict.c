/**
 * @file test-ucli.c
 *
 * @date May 2024
 * @author Enrico Dalla Croce (Kalsifer-742) [kalsifer742@gmail.com]
 */

#include "ucli-dictionary.h"
#include "unity.h"
#include <stdio.h>
#include <string.h>

uint8_t _ucli_dictionary_get_bucket(char* key);
void echo(int argc, char args[][10]) {}
ucli_dictionary_t dict;

void setUp(void) {}
void tearDown(void) {}
void test_unity(void) { TEST_ASSERT(1); }

void test_buckets_size(void) {
#define COMMANDS_N 30
#define BUCKETS_N 15
#define BUCKETS_SIZE 5

    char* commands[COMMANDS_N] = {
        "ls",    "cd",       "mkdir",   "rm",    "cp",   "mv",
        "chmod", "grep",     "cat",     "pwd",   "echo", "head",
        "tail",  "sort",     "uniq",    "wc",    "find", "tar",
        "gzip",  "gunzip",   "zip",     "unzip", "ssh",  "scp",
        "ping",  "ifconfig", "netstat", "ps",    "kill", "man"};

    int buckets[BUCKETS_N] = {0};

    for (size_t i = 0; i < COMMANDS_N; i++) {
        uint8_t bucket = _ucli_dictionary_get_bucket(commands[i]);
        buckets[bucket] = buckets[bucket] + 1;
    }

    for (size_t i = 0; i < BUCKETS_N; i++) {
        TEST_ASSERT_LESS_OR_EQUAL_INT32(BUCKETS_SIZE, buckets[i]);
    }
}

void test_dict_init(void) {
    ucli_dictionary_init(&dict);

    for (size_t i = 0; i < BUCKETS_N; i++) {
        for (size_t j = 0; j < BUCKETS_SIZE; j++) {
            for (size_t z = 0; z < UCLI_ARGS_LEN_MAX; z++) {
                TEST_ASSERT_EQUAL_CHAR('\0', dict.buckets[i][j].key[z]);
            }
            TEST_ASSERT_EQUAL_PTR(NULL, dict.buckets[i][j].function);
        }
    }
}

void test_dict_edit(void) {
    char* key = "echo";
    ucli_dictionary_return_code_t result = UCLI_DICTIONARY_RETURN_CODE_ERROR;
    ucli_command_function_t function = NULL;

    result = ucli_dictionary_get(&dict, key, NULL);
    TEST_ASSERT_EQUAL_INT(UCLI_DICTIONARY_RETURN_CODE_NOT_FOUND, result);

    result = ucli_dictionary_add(&dict, key, &echo);
    TEST_ASSERT_EQUAL_INT(UCLI_DICTIONARY_RETURN_CODE_OK, result);

    result = ucli_dictionary_get(&dict, key, &function);
    TEST_ASSERT_EQUAL_INT(UCLI_DICTIONARY_RETURN_CODE_OK, result);

    TEST_ASSERT_EQUAL_PTR(&echo, function);
}

void test_dict_contains(void) {
    char* key = "echo";
    ucli_dictionary_return_code_t result = UCLI_DICTIONARY_RETURN_CODE_ERROR;

    result = ucli_dictionary_contains_key(&dict, key);

    TEST_ASSERT_EQUAL_INT(UCLI_DICTIONARY_RETURN_CODE_OK, result);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_unity);

    RUN_TEST(test_buckets_size);

    RUN_TEST(test_dict_init);

    RUN_TEST(test_dict_edit);

    RUN_TEST(test_dict_contains);

    UNITY_END();
}
