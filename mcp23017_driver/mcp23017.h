/**
 * @file mcp23017.h
 * @version 1.0
 * @date 16/11/2023
 * @author Enrico Dalla Croce (Kalsifer-742)
 * 
 * @brief Driver to configure and operate the MCP23X17 I/O Expander
*/

///Include guards
#ifndef mcp_23017_h
#define mcp_23017_h

/// Includes
/// Import of I2C_HandleTypeDef and HAL_StatusTypeDef
#include "main.h"
/// Import additional types, like: uint8_t
#include <stdint.h>
/// Import bool type
#include <stdbool.h>

typedef struct {
    uint8_t bank,
    mirror,
    seqop,
    disslw,
    //haen_MCP23S17_only,
    odr,
    intpol;
} MCP23017_configuration_t;

MCP23017_configuration_t default_configuration = {
    .bank = 0,
    .mirror = 0,
    .seqop = 0,
    .disslw = 0,
    .odr = 0,
    .intpol = 0
};

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t device_address;
    uint8_t i2c_timeout;
} MCP23017_t;

typedef enum {
    port_a,
    port_b,
} port_t;

typedef enum {
    logic_high = 1,
    logic_low  = 0,
} logic_pin_level_t;

typedef enum {
    io_direction_input  = 1,
    io_direction_output = 0,
} io_direction_t;

typedef enum {
    input_polarity_inverted = 1,
    input_polarity_normal   = 0,
} input_polarity_mode_t;

typedef enum {
    interrupt_enabled  = 1,
    interrupt_disabled = 0,
} interrupt_status_t;

typedef enum {
    interrupt_compare_with_ref_value  = 1,
    interrupt_compare_with_prev_value = 0,
} interrupt_compare_mode_t;

typedef enum {
    pull_up_resistor_enabled = 1,
    pull_up_resistor_disable = 0,
} pull_up_resistor_status_t;

typedef enum {
    flag_high = 1,
    flag_low  = 0,
} flag_status_t;

/// Private
//uint8_t get_register_bit(uint8_t register_value, uint8_t index);
//void set_register_bit(uint8_t* register_value, uint8_t index, uint8_t bit_value);

/// Public

///Simple
HAL_StatusTypeDef get_config(MCP23017_t* hdev, MCP23017_configuration_t* config);
HAL_StatusTypeDef set_config(MCP23017_t* hdev, MCP23017_configuration_t config);
HAL_StatusTypeDef reset_config(MCP23017_t* hdev);
HAL_StatusTypeDef init(MCP23017_t* hdev, I2C_HandleTypeDef* hi2c, uint8_t device_address, MCP23017_configuration_t config);
HAL_StatusTypeDef init_default(MCP23017_t* hdev, I2C_HandleTypeDef* hi2c);
HAL_StatusTypeDef reset_bank_config(MCP23017_t* hdev);
HAL_StatusTypeDef get_io_direction_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, io_direction_t* direction);
HAL_StatusTypeDef set_io_direction_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, io_direction_t direction);
HAL_StatusTypeDef get_input_polarity_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, input_polarity_mode_t* mode);
HAL_StatusTypeDef set_input_polarity_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, input_polarity_mode_t mode);
HAL_StatusTypeDef get_interrupt_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_status_t* status);
HAL_StatusTypeDef set_interrupt_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_status_t status);
HAL_StatusTypeDef get_defval_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* defval);
HAL_StatusTypeDef set_defval_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t defval);
HAL_StatusTypeDef get_interrupt_compare_mode_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_compare_mode_t* mode);
HAL_StatusTypeDef set_interrupt_compare_mode_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_compare_mode_t mode);
HAL_StatusTypeDef get_pull_up_resistor_status_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, pull_up_resistor_status_t* status);
HAL_StatusTypeDef set_pull_up_resistor_status_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, pull_up_resistor_status_t status);
HAL_StatusTypeDef get_interrupt_flag_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, flag_status_t* status);
HAL_StatusTypeDef get_interrupt_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* status);
HAL_StatusTypeDef read_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* status);
HAL_StatusTypeDef write_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t value);

///Advanced
HAL_StatusTypeDef read_register(MCP23017_t* hdev, uint8_t register_address, uint8_t* register_value);
HAL_StatusTypeDef write_register(MCP23017_t* hdev, uint8_t register_address, uint8_t register_value);
HAL_StatusTypeDef read_register_bit(MCP23017_t* hdev, uint8_t register_address, uint8_t index, uint8_t* register_bit_value);
HAL_StatusTypeDef write_register_bit(MCP23017_t* hdev, uint8_t register_address, uint8_t index, uint8_t register_bit_value);
HAL_StatusTypeDef get_output_latch_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* status);
HAL_StatusTypeDef set_output_latch_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t status);

///Include guards
#endif