/**
 * @file test-cli.c
 * @brief 
 *
 * @date 26 Mar 2024
 * @author Name Surname [your@email.here]
 */

#include "unity.h"
#include "cli.h"

void setUp(void) {

}

void tearDown(void) {

}

void test_unity(void) {
    TEST_ASSERT(1);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_unity);

    UNITY_END();
}

