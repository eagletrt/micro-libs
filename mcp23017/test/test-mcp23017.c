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

void check_mcp23017_get_register_bit(void) {
    uint8_t register_value = 0b00001000;
    uint8_t bit_value = 0;
    
    for (uint8_t i = 0; i < 8; i++)
    {
        bit_value = mcp23017_get_register_bit(register_value, i);
        if (i == 3)
        {
            TEST_ASSERT_EQUAL_UINT8(1, bit_value);
        } else {
            TEST_ASSERT_EQUAL_UINT8(0, bit_value);
        }
    }
    
    //check register value is not changed
    TEST_ASSERT_EQUAL_UINT8(0b00001000, register_value);
}

void check_mcp23017_set_register_bit(void) {
    uint8_t register_value = 0b00000000;
    
    for (uint8_t i = 0; i < 8; i++)
    {
        mcp23017_set_register_bit(&register_value, i, 1);
    }
    TEST_ASSERT_EQUAL_UINT8(0b11111111, register_value);

    for (uint8_t i = 0; i < 8; i++)
    {
        mcp23017_set_register_bit(&register_value, i, 0);
    }
    TEST_ASSERT_EQUAL_UINT8(0b00000000, register_value);
}

void check_mcp23017_set_it_on_pin(void) {
    uint8_t gpinten_register_value = 0b00000000;
    uint8_t intcon_register_value = 0b00000000;
    uint8_t defval_register_value = 0b00000000;

    //general check
    mcp23017_set_it_on_pin(
        &gpinten_register_value,
        &intcon_register_value,
        &defval_register_value,
        3,
        MCP23017_INT_ENABLED,
        MCP23017_INT_MODE_COMPARE,
        0
    );
    TEST_ASSERT_EQUAL_UINT8(0b00001000, gpinten_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00001000, intcon_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, defval_register_value);

    //reset values check
    mcp23017_set_it_on_pin(
        &gpinten_register_value,
        &intcon_register_value,
        &defval_register_value,
        3,
        MCP23017_INT_DISABLED,
        MCP23017_INT_MODE_ON_CHANGE,
        0
    );
    TEST_ASSERT_EQUAL_UINT8(0b00000000, gpinten_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, intcon_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, defval_register_value);
}

void check_mcp23017_set_it_on_pin_it_mode(void) {
    uint8_t gpinten_register_value = 0b00000000;
    uint8_t intcon_register_value = 0b00000000;
    uint8_t defval_register_value = 0b00000000;
    
    mcp23017_set_it_on_pin(
        &gpinten_register_value,
        &intcon_register_value,
        &defval_register_value,
        3,
        MCP23017_INT_DISABLED,
        MCP23017_INT_MODE_COMPARE,
        1
    );
    TEST_ASSERT_EQUAL_UINT8(0b00000000, gpinten_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00001000, intcon_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00001000, defval_register_value);

    mcp23017_set_it_on_pin(
        &gpinten_register_value,
        &intcon_register_value,
        &defval_register_value,
        3,
        MCP23017_INT_DISABLED,
        MCP23017_INT_MODE_ON_CHANGE,
        0 //this change should be ignored
    );
    TEST_ASSERT_EQUAL_UINT8(0b00000000, gpinten_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, intcon_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00001000, defval_register_value);
}

void check_mcp23017_set_it_on_all_pins(void) {
    uint8_t gpinten_register_value = 0b00000000;
    uint8_t intcon_register_value = 0b00000000;
    uint8_t defval_register_value = 0b00000000;

    mcp23017_set_it_on_all_pins(
        &gpinten_register_value,
        &intcon_register_value,
        &defval_register_value,
        MCP23017_INT_ENABLED,
        MCP23017_INT_MODE_ON_CHANGE,
        0b11111111
    );
    TEST_ASSERT_EQUAL_UINT8(0b11111111, gpinten_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, intcon_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, defval_register_value);

    mcp23017_set_it_on_all_pins(
        &gpinten_register_value,
        &intcon_register_value,
        &defval_register_value,
        MCP23017_INT_ENABLED,
        MCP23017_INT_MODE_COMPARE,
        0b00111111
    );
    TEST_ASSERT_EQUAL_UINT8(0b11111111, gpinten_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b11111111, intcon_register_value);
    TEST_ASSERT_EQUAL_UINT8(0b00111111, defval_register_value);
}

void check_set_mirror_bit(void) {
    uint8_t iocon_register_value = 0b00000000;
    
    mcp23017_set_register_bit(&iocon_register_value, MCP23017_MIRROR_BIT, MCP23017_INT_MIRROR_ENABLED);
    TEST_ASSERT_EQUAL_UINT8(0b01000000, iocon_register_value);

    mcp23017_set_register_bit(&iocon_register_value, MCP23017_MIRROR_BIT, MCP23017_INT_MIRROR_DISABLED);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, iocon_register_value);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(check_mcp23017_get_register_bit);
    RUN_TEST(check_mcp23017_set_register_bit);
    RUN_TEST(check_mcp23017_set_it_on_pin);
    RUN_TEST(check_mcp23017_set_it_on_pin_it_mode);
    RUN_TEST(check_mcp23017_set_it_on_all_pins);
    RUN_TEST(check_set_mirror_bit);

    UNITY_END();
}