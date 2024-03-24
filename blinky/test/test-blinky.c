/**
 * @file test-blinky.c
 * @brief Test if the blinky library works
 *
 * @date 14 Dec 2023
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 */

#include "unity.h"
#include "blinky.h"

#define PATTERN_SIZE 3
#define OTHER_PATTERN_SIZE 2

uint16_t pattern[PATTERN_SIZE] = {
    1000,   // HIGH
    1000,   // LOW
    1000    // HIGH
};
uint16_t otherPattern[OTHER_PATTERN_SIZE] = { 500, 2500 };

Blinky bOnce;
Blinky bRepeat;

void setUp(void) {
    blinky_init(&bOnce, pattern, PATTERN_SIZE, false, BLINKY_HIGH);
    blinky_init(&bRepeat, pattern, PATTERN_SIZE, true, BLINKY_HIGH);
}

void tearDown(void) {

}

void check_blink_initial_state(void) {
    TEST_ASSERT_EQUAL_INT(BLINKY_HIGH, bOnce.initial_state);
}

void check_blink_enabled(void) {
    blinky_enable(&bOnce, true);
    TEST_ASSERT_TRUE(bOnce.enable);
}
void check_blink_disabled(void) {
    blinky_enable(&bOnce, false);
    TEST_ASSERT_FALSE(bOnce.enable);
}

void check_blink_repeated(void) {
    blinky_repeat(&bOnce, true);
    TEST_ASSERT_TRUE(bOnce.repeat);
}
void check_blink_once(void) {
    blinky_repeat(&bOnce, false);
    TEST_ASSERT_FALSE(bOnce.repeat);
}

void check_blink_set_pattern_size(void) {
    blinky_set_pattern(&bOnce, otherPattern, OTHER_PATTERN_SIZE);
    TEST_ASSERT_EQUAL_UINT8(OTHER_PATTERN_SIZE, bOnce.size);
}
void check_blink_set_pattern_data(void) {
    blinky_set_pattern(&bOnce, otherPattern, OTHER_PATTERN_SIZE);
    TEST_ASSERT_EQUAL_UINT16_ARRAY(otherPattern, bOnce.pattern, OTHER_PATTERN_SIZE);
}

void check_blink_reset_time(void) {
    blinky_reset(&bOnce, BLINKY_LOW);
    TEST_ASSERT_EQUAL_UINT32(bOnce.t, 0U);
}
void check_blink_reset_index(void) {
    blinky_reset(&bOnce, BLINKY_LOW);
    TEST_ASSERT_EQUAL_UINT8(bOnce.index, 0U);
}
void check_blink_reset_state(void) {
    blinky_reset(&bOnce, BLINKY_LOW);
    TEST_ASSERT_EQUAL_INT(bOnce.state, BLINKY_LOW);
}
void check_blink_reset_enable(void) {
    blinky_reset(&bOnce, BLINKY_LOW);
    TEST_ASSERT_TRUE(bOnce.enable);
}

void check_blink_routine_with_null_handler(void) {
    TEST_ASSERT_EQUAL_INT(BLINKY_LOW, blinky_routine(NULL, 0));
}
void check_blink_routine_with_null_pattern(void) {
    bOnce.pattern = NULL;
    TEST_ASSERT_EQUAL_INT(BLINKY_LOW, blinky_routine(&bOnce, 0));
}
void check_blink_routine_when_disabled(void) {
    bOnce.enable = false;
    bOnce.state = BLINKY_LOW;
    TEST_ASSERT_EQUAL_INT(BLINKY_LOW, blinky_routine(&bOnce, 0));
}
void check_blink_routine_start_high(void) {
    TEST_ASSERT_EQUAL_INT(BLINKY_HIGH, blinky_routine(&bOnce, 0));
}
void check_blink_routine_change_state(void) {
    TEST_ASSERT_EQUAL_INT(BLINKY_HIGH, blinky_routine(&bOnce, 500));
    TEST_ASSERT_EQUAL_INT(BLINKY_LOW, blinky_routine(&bOnce, 1500));
}
void check_blink_routine_runned_once(void) {
    bOnce.index = 2;
    blinky_routine(&bOnce, 999);
    TEST_ASSERT_TRUE(bOnce.enable);
    blinky_routine(&bOnce, 1001);
    TEST_ASSERT_FALSE(bOnce.enable);
}
void check_blink_routine_repeated_state(void) {
    bRepeat.index = 2;
    TEST_ASSERT_EQUAL_INT(BLINKY_HIGH, blinky_routine(&bRepeat, 999));
    TEST_ASSERT_EQUAL_INT(bRepeat.initial_state, blinky_routine(&bRepeat, 1001));
}
void check_blink_routine_repeated_enable(void) {
    bRepeat.index = 2;
    blinky_routine(&bRepeat, 1001);
    TEST_ASSERT_TRUE(bRepeat.enable);
}
void check_blink_routine_repeated_index(void) {
    bRepeat.index = 2;
    blinky_routine(&bRepeat, 1001);
    TEST_ASSERT_EQUAL_UINT8(0, bRepeat.index);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(check_blink_initial_state);

    RUN_TEST(check_blink_enabled);
    RUN_TEST(check_blink_disabled);

    RUN_TEST(check_blink_repeated);
    RUN_TEST(check_blink_once);

    RUN_TEST(check_blink_set_pattern_size);
    RUN_TEST(check_blink_set_pattern_data);

    RUN_TEST(check_blink_reset_time);
    RUN_TEST(check_blink_reset_index);
    RUN_TEST(check_blink_reset_state);
    RUN_TEST(check_blink_reset_enable);

    RUN_TEST(check_blink_routine_with_null_handler);
    RUN_TEST(check_blink_routine_with_null_pattern);
    RUN_TEST(check_blink_routine_when_disabled);
    RUN_TEST(check_blink_routine_start_high);
    RUN_TEST(check_blink_routine_change_state);
    RUN_TEST(check_blink_routine_runned_once);
    RUN_TEST(check_blink_routine_repeated_state);
    RUN_TEST(check_blink_routine_repeated_enable);
    RUN_TEST(check_blink_routine_repeated_index);

    UNITY_END();
}

