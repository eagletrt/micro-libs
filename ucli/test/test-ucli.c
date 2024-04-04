/**
 * @file test-ucli.c
 * @brief 
 *
 * @date 04 Apr 2024
 * @author Name Surname [your@email.here]
 */

#include "unity.h"
#include "ucli.h"

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

