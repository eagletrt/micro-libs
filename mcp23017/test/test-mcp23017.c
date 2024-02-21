/**
 * @file test-mcp23017.c
 * @brief Unit test for the MCP23017 I/O Expander Driver
 *
 * @date 21 Feb 2024
 * @author Enrico Dalla Croce (Kalsifer-742) [kalsifer742@gmail.com]
 */

#include "unity.h"
#include "mcp23017.h"

void setUp(void) {

}

void tearDown(void) {

}

void check_get_register_bit(void) {
    uint8_t register_value = 0b00001000;
    uint8_t bit_value = 0;

    bit_value = get_register_bit(register_value, 3);
    TEST_ASSERT_EQUAL_UINT8(1, bit_value);
    //check register value is not changed
    TEST_ASSERT_EQUAL_UINT8(0b00001000, register_value);
}

void check_get_register_bit_indexes(void) {
    uint8_t register_value = 0b10001000;
    uint8_t bit_value = 0;

    bit_value = get_register_bit(register_value, 0);
    TEST_ASSERT_EQUAL_UINT8(0, bit_value);
    //check register value is not changed
    TEST_ASSERT_EQUAL_UINT8(0b10001000, register_value);

    bit_value = get_register_bit(register_value, 7);
    TEST_ASSERT_EQUAL_UINT8(1, bit_value);
    //check register value is not changed
    TEST_ASSERT_EQUAL_UINT8(0b10001000, register_value);
}

void check_set_register_bit(void) {
    uint8_t register_value = 0b00000000;
    
    set_register_bit(&register_value, 3, 1);
    TEST_ASSERT_EQUAL_UINT8(0b00001000, register_value);

    set_register_bit(&register_value, 5, 1);
    TEST_ASSERT_EQUAL_UINT8(0b00101000, register_value);

    set_register_bit(&register_value, 3, 0);
    TEST_ASSERT_EQUAL_UINT8(0b00100000, register_value);
}

void check_set_register_bit_indexes(void) {
    uint8_t register_value = 0b00000000;
    
    set_register_bit(&register_value, 0, 1);
    TEST_ASSERT_EQUAL_UINT8(0b00000001, register_value);

    set_register_bit(&register_value, 7, 1);
    TEST_ASSERT_EQUAL_UINT8(0b10000001, register_value);

    set_register_bit(&register_value, 0, 0);
    TEST_ASSERT_EQUAL_UINT8(0b10000000, register_value);

    set_register_bit(&register_value, 7, 0);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, register_value);
}

void check_set_interrupt_on_pin(void) {
    uint8_t gpinten_register_value = 0b00000000;
    uint8_t intcon_register_value = 0b00000000;
    uint8_t defval_register_value = 0b00000000;
    uint8_t iocon_register_value = 0b00000000;
    
    //general check
    set_interrupt_on_pin(
        &gpinten_register_value,
        &intcon_register_value,
        &defval_register_value,
        &iocon_register_value,
        3,
        interrupt_setting_enabled,
        interrupt_mode_compare,
        0,
        interrupt_mirror_enabled
    );
    TEST_ASSERT_EQUAL_UINT8(0b00001000, gpinten_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00001000, intcon_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, defval_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b10000000, iocon_register_value);

    //reset values check
    set_interrupt_on_pin(
        &gpinten_register_value,
        &intcon_register_value,
        &defval_register_value,
        &iocon_register_value,
        3,
        interrupt_setting_disabled,
        interrupt_mode_on_change,
        0,
        interrupt_mirror_disabled
    );
    TEST_ASSERT_EQUAL_UINT8(0b00000000, gpinten_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, intcon_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, defval_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, iocon_register_value);
}

void check_set_interrupt_on_pin_register_consistency(void) {
    uint8_t gpinten_register_value = 0b00001000;
    uint8_t intcon_register_value = 0b00001000;
    uint8_t defval_register_value = 0b00001000;
    uint8_t iocon_register_value = 0b00001000;

    //check that the register is not cleared
    set_interrupt_on_pin(
        &gpinten_register_value,
        &intcon_register_value,
        &defval_register_value,
        &iocon_register_value,
        5,
        interrupt_setting_enabled,
        interrupt_mode_compare,
        1,
        interrupt_mirror_enabled
    );
    TEST_ASSERT_EQUAL_UINT8(0b00101000, gpinten_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00101000, intcon_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00101000, defval_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b10001000, iocon_register_value);
}

void check_set_interrupt_on_pin_mode_setting(void) {
    uint8_t gpinten_register_value = 0b00000000;
    uint8_t intcon_register_value = 0b00000000;
    uint8_t defval_register_value = 0b00000000;
    uint8_t iocon_register_value = 0b00000000;
    
    set_interrupt_on_pin(
        &gpinten_register_value,
        &intcon_register_value,
        &defval_register_value,
        &iocon_register_value,
        3,
        interrupt_setting_disabled,
        interrupt_mode_compare,
        1,
        interrupt_mirror_disabled
    );
    TEST_ASSERT_EQUAL_UINT8(0b00000000, gpinten_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00001000, intcon_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00001000, defval_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, iocon_register_value);

    set_interrupt_on_pin(
        &gpinten_register_value,
        &intcon_register_value,
        &defval_register_value,
        &iocon_register_value,
        3,
        interrupt_setting_disabled,
        interrupt_mode_on_change,
        0, //this change should be ignored
        interrupt_mirror_disabled
    );
    TEST_ASSERT_EQUAL_UINT8(0b00000000, gpinten_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, intcon_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00001000, defval_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, iocon_register_value);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(check_get_register_bit);
    RUN_TEST(check_get_register_bit_indexes);
    RUN_TEST(check_set_register_bit);
    RUN_TEST(check_set_register_bit_indexes);
    RUN_TEST(check_set_interrupt_on_pin);
    RUN_TEST(check_set_interrupt_on_pin_register_consistency);
    RUN_TEST(check_set_interrupt_on_pin_mode_setting);

    UNITY_END();
}