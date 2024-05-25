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

void setUp(void) {}
void tearDown(void) {}
void test_unity(void) { TEST_ASSERT(1); }

#define COMMANDS_N 30
void test_buckets(void) {
    char* commands[COMMANDS_N] = {
        "ls",    "cd",       "mkdir",   "rm",    "cp",   "mv",
        "chmod", "grep",     "cat",     "pwd",   "echo", "head",
        "tail",  "sort",     "uniq",    "wc",    "find", "tar",
        "gzip",  "gunzip",   "zip",     "unzip", "ssh",  "scp",
        "ping",  "ifconfig", "netstat", "ps",    "kill", "man"};

    struct bucket {
        int bucket;
        int counter;
    } int buckets_count[COMMANDS_N][1];

    for (size_t i = 0; i < COMMANDS_N; i++) {
        buckets_count[i] = _ucli_dictionary_get_bucket(commands[i]);
        char buffer[10];
        sprintf(buffer, "%d", buckets[i]);
        TEST_MESSAGE(buffer);
    }

    for (size_t i = 0; i < COMMANDS_N; i++) {
        for (size_t j = 0; j < COMMANDS_N; j++) {
            if (i != j) {
                if (buckets[i] == buckets[j]) {
                    /* code */
                }
            }

            TEST_ASSERT_NOT_EQUAL_UINT8(buckets[i], buckets[j]);
        }
    }
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_unity);

    RUN_TEST(test_buckets);

    UNITY_END();
}
