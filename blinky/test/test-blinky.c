/**
 * @file test-blinky.c
 * @brief Test if the blinky library works
 *
 * @date 14 Dec 2023
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 */

#include "unity.h"
#include "blinky.h"

uint16_t pattern[] = {
    1000,   // HIGH
    1000,   // LOW
    1000    // HIGH
};
uint16_t otherPattern[] = { 500, 2500 };

Blinky bOnce;
Blinky bRepeat;

char * msg_blink_disabled = "Blinker not enabled";
char * msg_index_reset = "Pattern index not reset correctly";

void setUp(void) {
    blinky_init(&bOnce, pattern, 3, false);
    blinky_init(&bRepeat, pattern, 3, true);
}

void tearDown(void) {

}

void is_blink_enabled(void) {
    blinky_enable(&bOnce, true);
    TEST_ASSERT_TRUE(bOnce.enable);
}
void is_blink_disabled(void) {
    blinky_enable(&bOnce, false);
    TEST_ASSERT_FALSE(bOnce.enable);
}

void is_blink_repeated(void) {
    blinky_repeat(&bOnce, true);
    TEST_ASSERT_TRUE(bOnce.repeat);
}
void is_blink_once(void) {
    blinky_repeat(&bOnce, false);
    TEST_ASSERT_FALSE(bOnce.repeat);
}

void is_pattern_set(void) {
    blinky_set_pattern(&bOnce, otherPattern);
    TEST_ASSERT_EQUAL_UINT16_ARRAY(otherPattern, bOnce.pattern, 2);
}

void is_blink_reset(void) {
    blinky_reset(&bOnce);
    TEST_ASSERT_EQUAL_INT(BLINKY_HIGH, bOnce.state);
    TEST_ASSERT_TRUE_MESSAGE(bOnce.enable, msg_blink_disabled);
}

void routine_with_null_pattern(void) {
    bOnce.pattern = NULL;
    TEST_ASSERT_EQUAL_INT(BLINKY_LOW, blinky_routine(&bOnce, 0));
}
void routine_when_disabled(void) {
    bOnce.enable = false;
    TEST_ASSERT_EQUAL_INT(BLINKY_LOW, blinky_routine(&bOnce, 0));
}
void routine_start_high(void) {
    TEST_ASSERT_EQUAL_INT(BLINKY_HIGH, blinky_routine(&bOnce, 0));
}
void routine_change_state(void) {
    TEST_ASSERT_EQUAL_INT(BLINKY_HIGH, blinky_routine(&bOnce, 500));
    TEST_ASSERT_EQUAL_INT(BLINKY_LOW, blinky_routine(&bOnce, 1500));
}
void routine_is_runned_once(void) {
    bOnce.index = 2;
    blinky_routine(&bOnce, 999);
    TEST_ASSERT_TRUE(bOnce.enable);
    blinky_routine(&bOnce, 1001);
    TEST_ASSERT_FALSE(bOnce.enable);
}
void routine_is_repeated(void) {
    bRepeat.index = 2;
    TEST_ASSERT_EQUAL_INT(BLINKY_HIGH, blinky_routine(&bRepeat, 999));
    TEST_ASSERT_EQUAL_INT(BLINKY_HIGH, blinky_routine(&bRepeat, 1001));

    TEST_ASSERT_TRUE_MESSAGE(bRepeat.enable, msg_blink_disabled);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, bRepeat.index, msg_index_reset);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(is_blink_enabled);
    RUN_TEST(is_blink_disabled);

    RUN_TEST(is_blink_repeated);
    RUN_TEST(is_blink_once);

    RUN_TEST(is_pattern_set);

    RUN_TEST(is_blink_reset);

    RUN_TEST(routine_with_null_pattern);
    RUN_TEST(routine_when_disabled);
    RUN_TEST(routine_start_high);
    RUN_TEST(routine_change_state);
    RUN_TEST(routine_is_runned_once);
    RUN_TEST(routine_is_repeated);

    UNITY_END();
}

