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
    uint8_t* iocon_register_value,
    uint8_t pin_number,
    uint8_t interrupt_setting,
    uint8_t interrupt_mode,
    uint8_t compare_value,
    uint8_t mirror_setting
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
    // set mirror setting
    mcp23017_set_register_bit(iocon_register_value, 7, mirror_setting);
}
