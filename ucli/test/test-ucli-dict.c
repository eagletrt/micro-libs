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

#undef COMMANDS_N
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

#undef BUCKETS_N
#undef BUCKETS_SIZE
}

void test_dict_add(void) {
    ucli_dictionary_return_code_t result = UCLI_DICTIONARY_RETURN_CODE_ERROR;
    char* key = "echo";

    result = ucli_dictionary_add(&dict, key, &echo);
    TEST_ASSERT_EQUAL_INT(UCLI_DICTIONARY_RETURN_CODE_OK, result);
}

void test_dict_get(void) {
    ucli_dictionary_return_code_t result = UCLI_DICTIONARY_RETURN_CODE_ERROR;
    char* key = "echo";
    ucli_command_function_t function = NULL;

    result = ucli_dictionary_get(&dict, key, &function);
    TEST_ASSERT_EQUAL_INT(UCLI_DICTIONARY_RETURN_CODE_OK, result);

    TEST_ASSERT_EQUAL_PTR(&echo, function);
}

void test_dict_contains(void) {
    ucli_dictionary_return_code_t result = UCLI_DICTIONARY_RETURN_CODE_ERROR;
    char* key = "echo";

    result = ucli_dictionary_contains_key(&dict, key);

    TEST_ASSERT_EQUAL_INT(UCLI_DICTIONARY_RETURN_CODE_OK, result);
}

void chmod(int argc, char args[][10]) {}
void ifconfig(int argc, char args[][10]) {}
void cat(int argc, char args[][10]) {}
void test_dict_probing(void) {
#define COMMANDS_N 3
#define COLLISION_BUCKET 12

    ucli_dictionary_return_code_t result = UCLI_DICTIONARY_RETURN_CODE_ERROR;

    ucli_command_t commands[COMMANDS_N] = {
        {.name = "chmod", .function = &chmod},
        {.name = "ifconfig", .function = &ifconfig},
        {.name = "cat", .function = &cat}};

    for (size_t i = 0; i < COMMANDS_N; i++) {
        result =
            ucli_dictionary_add(&dict, commands[i].name, commands[i].function);
        TEST_ASSERT_EQUAL_INT(UCLI_DICTIONARY_RETURN_CODE_OK, result);
    }

    for (size_t i = 0; i < COMMANDS_N; i++) {
        TEST_ASSERT_EQUAL_CHAR_ARRAY(commands[i].name,
                                     dict.buckets[COLLISION_BUCKET][i].key, 10);
        TEST_ASSERT_EQUAL_PTR(commands[i].function,
                              dict.buckets[COLLISION_BUCKET][i].function);
    }

    for (size_t i = 0; i < COMMANDS_N; i++) {
        result = ucli_dictionary_contains_key(&dict, commands[i].name);
        TEST_ASSERT_EQUAL_INT(UCLI_DICTIONARY_RETURN_CODE_OK, result);
    }

    for (size_t i = 0; i < COMMANDS_N; i++) {
        ucli_command_function_t function = NULL;
        result = ucli_dictionary_get(&dict, commands[i].name, &function);
        TEST_ASSERT_EQUAL_INT(UCLI_DICTIONARY_RETURN_CODE_OK, result);
        TEST_ASSERT_EQUAL_PTR(commands[i].function, function);
    }

#undef COMMANDS_N
#undef COLLISION_BUCKET
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_unity);

    RUN_TEST(test_buckets_size);

    RUN_TEST(test_dict_init);

    RUN_TEST(test_dict_add);

    RUN_TEST(test_dict_get);

    RUN_TEST(test_dict_contains);

    RUN_TEST(test_dict_probing);

    UNITY_END();
}
