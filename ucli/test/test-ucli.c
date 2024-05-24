/**
 * @file test-ucli.c
 *
 * @date May 2024
 * @author Enrico Dalla Croce (Kalsifer-742) [kalsifer742@gmail.com]
 */

#include "ucli-dictionary.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_unity(void) { TEST_ASSERT(1); }

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_unity);

    UNITY_END();
}
