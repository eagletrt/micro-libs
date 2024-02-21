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

// MCP23017 Registers Address
#define REGISTER_IODIRA   0x00
#define REGISTER_IODIRB   0x01
#define REGISTER_IPOLA    0x02
#define REGISTER_IPOLB    0x03
#define REGISTER_GPINTENA 0x04
#define REGISTER_GPINTENB 0x05 
#define REGISTER_DEFVALA  0x06
#define REGISTER_DEFVALB  0x07
#define REGISTER_INTCONA  0x08
#define REGISTER_INTCONB  0x09
#define REGISTER_IOCON    0x0A
/*
#define REGISTER_IOCONA = 0x0A
#define REGISTER_IOCONB = 0x0B
*/
#define REGISTER_GPPUA    0x0C
#define REGISTER_GPPUB    0x0D
#define REGISTER_INTFA    0x0E
#define REGISTER_INTFB    0x0F
#define REGISTER_INTCAPA  0x10
#define REGISTER_INTCAPB  0x11
#define REGISTER_GPIOA    0x12
#define REGISTER_GPIOB    0x13
#define REGISTER_OLATA    0x14
#define REGISTER_OLATB    0x15

#define MCP23017_I2C_SIZE 0x01

typedef enum {
    interrupt_setting_disabled,
    interrupt_setting_enabled
} INTERRUPT_SETTING_t;

typedef enum {
    interrupt_mode_on_change,
    interrupt_mode_compare
} INTERRUPT_MODE_t;

typedef enum {
    interrupt_mirror_disabled,
    interrupt_mirror_enabled
} MIRROR_SETTING_t;

uint8_t get_register_bit(uint8_t register_value, uint8_t index);
void set_register_bit(uint8_t* register_value, uint8_t index, uint8_t bit_value);
void set_interrupt_on_pin(
    uint8_t* gpinten_register_value,
    uint8_t* intcon_register_value,
    uint8_t* defval_register_value,
    uint8_t* iocon_register_value,
    uint8_t pin,
    INTERRUPT_SETTING_t interrupt_setting,
    INTERRUPT_MODE_t interrupt_mode,
    uint8_t compare_value,
    MIRROR_SETTING_t mirros_setting
);

#endif  // MCP23017_H