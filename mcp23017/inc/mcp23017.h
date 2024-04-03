/**
 * @file mcp23017.h
 * @brief Driver to configure and operate the MCP23017 I/O Expander
 *
 * @date 21 Feb 2024
 * @author Enrico Dalla Croce (Kalsifer-742) [kalsifer742@gmail.com]
 */

#ifndef MCP23017_H
#define MCP23017_H

#include <stdint.h>

/**
 * @paragraph MCP23017 Registers Address
 * 
 * Remember that each port have its addresses.
 */

#define MCP23017_REGISTER_IODIRA   0x00
#define MCP23017_REGISTER_IODIRB   0x01
#define MCP23017_REGISTER_IPOLA    0x02
#define MCP23017_REGISTER_IPOLB    0x03
#define MCP23017_REGISTER_GPINTENA 0x04
#define MCP23017_REGISTER_GPINTENB 0x05 
#define MCP23017_REGISTER_DEFVALA  0x06
#define MCP23017_REGISTER_DEFVALB  0x07
#define MCP23017_REGISTER_INTCONA  0x08
#define MCP23017_REGISTER_INTCONB  0x09
#define MCP23017_REGISTER_IOCON    0x0A
/*
#define REGISTER_IOCONA = 0x0A
#define REGISTER_IOCONB = 0x0B
*/
#define MCP23017_REGISTER_GPPUA    0x0C
#define MCP23017_REGISTER_GPPUB    0x0D
#define MCP23017_REGISTER_INTFA    0x0E
#define MCP23017_REGISTER_INTFB    0x0F
#define MCP23017_REGISTER_INTCAPA  0x10
#define MCP23017_REGISTER_INTCAPB  0x11
#define MCP23017_REGISTER_GPIOA    0x12
#define MCP23017_REGISTER_GPIOB    0x13
#define MCP23017_REGISTER_OLATA    0x14
#define MCP23017_REGISTER_OLATB    0x15

/**
 * @paragraph MCP23017 settings
 */
#define MCP23017_I2C_SIZE 0x01
// #define MCP23017_PORTA 0x00
// #define MCP23017_PORTB 0x01
#define MCP23017_PINS_N 0x07
#define MCP23017_MIRROR_BIT 0x06

/**
 * @paragraph MCP23017 function parameters
 */
#define MCP23017_INT_DISABLED 0x00
#define MCP23017_INT_ENABLED 0x01
#define MCP23017_INT_MODE_ON_CHANGE 0x00
#define MCP23017_INT_MODE_COMPARE 0x01
#define MCP23017_INT_MIRROR_DISABLED 0x00
#define MCP23017_INT_MIRROR_ENABLED 0x01

/**
 * @brief Get the value of a specific bit of a register
 *
 * @param register_value Value of the register you want to read the bit from.
 * @param index Bit index counted from right to left. range `0-7`.
 * @return Value of the specified bit.
 */
uint8_t mcp23017_get_register_bit(uint8_t register_value, uint8_t index);

/**
 * @brief Set the value of a specific bit of a register
 *
 * @param register_value Pointer to the value of the register you want to write to.
 * @param index Bit index counted from right to left. range `0-7`.
 * @param bit_value Value to assign to the specified bit.
 */
void mcp23017_set_register_bit(uint8_t* register_value, uint8_t index, uint8_t bit_value);

/**
 * @brief Sets interrupt settings for a specific pin.
 *
 * @param gpinten_register_value Pointer to the `GPINTEN` register value.
 * @param intcon_register_value Pointer to the `INTCON` register value.
 * @param defval_register_value Pointer to the `DEFVAL` register value.
 * @param iocon_register_value Pointer to the `IOCON` register value.
 * @param pin_number Pin number counted from right to left. range `0-7`.
 * @param interrupt_setting Interrupt setting for the pin. `MCP23017_INT_DISABLED` `MCP23017_INT_ENABLED`.
 * @param interrupt_mode Interrupt mode for the pin. `MCP23017_INT_MODE_ON_CHANGE`: Interrupt triggered on any change on the pin state. `MCP23017_INT_MODE_COMPARE`: Interrupt triggered when the value on the pin differs from the reference value of the pin stored in the DEFVAL register.
 * @param compare_value Reference value for the pin. `0` or `1`.
 */
void mcp23017_set_it_on_pin(
    uint8_t* gpinten_register_value,
    uint8_t* intcon_register_value,
    uint8_t* defval_register_value,
    uint8_t pin_number,
    uint8_t interrupt_setting,
    uint8_t interrupt_mode,
    uint8_t compare_value
);

/**
 * @brief Sets interrupt settings for all pins of a port.
 * The parameters are the same of the `mcp23017_set_it_on_pin()` function.
 * The difference is that the setting is applied to every pin.
 * If you want to use `MCP23017_INT_MODE_COMPARE` you need to explicit a compare value for all pins.
 * Ex: `uint8_t compare_value = 0b11110000` (this means that the default value for pin `7-4` is set to `1` and `0` for the rest)
*/
void mcp23017_set_it_on_all_pins(
    uint8_t* gpinten_register_value,
    uint8_t* intcon_register_value,
    uint8_t* defval_register_value,
    uint8_t interrupt_setting,
    uint8_t interrupt_mode,
    uint8_t compare_value
);

#endif  // MCP23017_H