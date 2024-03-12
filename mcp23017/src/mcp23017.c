/**
 * @file mcp23017.c
 * @brief Driver to configure and operate the MCP23017 I/O Expander
 *
 * @date 21 Feb 2024
 * @author Enrico Dalla Croce (Kalsifer-742) [kalsifer742@gmail.com]
 */

#include "mcp23017.h"

uint8_t mcp23017_get_register_bit(uint8_t register_value, uint8_t index) {
    uint8_t bit_value = (register_value >> index) & 1;

    return bit_value;
}

void mcp23017_set_register_bit(uint8_t* register_value, uint8_t index, uint8_t bit_value) {
    if (bit_value == 0) {
        *register_value = *register_value & ~(1 << index);
    } else if (bit_value == 1) {
        *register_value = *register_value | (1 << index);
    }
}

void mcp23017_set_it_on_pin(
    uint8_t* gpinten_register_value,
    uint8_t* intcon_register_value,
    uint8_t* defval_register_value,
    uint8_t pin_number,
    uint8_t interrupt_setting,
    uint8_t interrupt_mode,
    uint8_t compare_value
) {
    // enable/disable interrupt on pin
    mcp23017_set_register_bit(gpinten_register_value, pin_number, interrupt_setting);
    // set interrupt mode on pin
    mcp23017_set_register_bit(intcon_register_value, pin_number, interrupt_mode);
    // if interrupt mode is compare
    if (interrupt_mode == MCP23017_INT_MODE_COMPARE)
    {
        // set compare value
        mcp23017_set_register_bit(defval_register_value, pin_number, compare_value);
    }
}

void mcp23017_set_it_on_all_pins(
    uint8_t* gpinten_register_value,
    uint8_t* intcon_register_value,
    uint8_t* defval_register_value,
    uint8_t interrupt_setting,
    uint8_t interrupt_mode,
    uint8_t compare_value
) {
    for (uint8_t pin_n = 0; pin_n <= MCP23017_PINS_N; pin_n++)
    {
        mcp23017_set_it_on_pin(
            gpinten_register_value,
            intcon_register_value,
            defval_register_value,
            pin_n,
            interrupt_setting,
            interrupt_mode,
            mcp23017_get_register_bit(compare_value, pin_n)
        );
    }
}