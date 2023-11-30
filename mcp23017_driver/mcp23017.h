/**
 * @author Enrico Dalla Croce (Kalsifer-742)
 * @date 30/11/2023
 * 
 * @brief Driver to configure and operate the MCP23017 I/O Expander
*/

#ifndef mcp_23017_h
#define mcp_23017_h

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

#define GET_VALUE_ON_PIN(function_name, REGISTER_A, REGISTER_B, output_parameter_name)                                                          \
    HAL_StatusTypeDef get##_##function_name##_##on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* output_parameter_name) {    \
        HAL_StatusTypeDef HAL_Status;                                                                                                           \
        uint8_t bit_value;                                                                                                                      \
                                                                                                                                                \
        if (port == MCP23017_PORTA) {                                                                                                           \
            HAL_Status = read_register_bit(hdev, REGISTER_A, pin_number, &bit_value);                                                           \
        } else if (port == MCP23017_PORTB) {                                                                                                    \
            HAL_Status = read_register_bit(hdev, REGISTER_B, pin_number, &bit_value);                                                           \
        }                                                                                                                                       \
                                                                                                                                                \
        if (HAL_Status == HAL_OK) {                                                                                                             \
            *output_parameter_name = bit_value;                                                                                                 \
        }                                                                                                                                       \
                                                                                                                                                \
        return HAL_Status;                                                                                                                      \
    }                                                                                                                                           \

#define SET_VALUE_ON_PIN(function_name, REGISTER_A, REGISTER_B, input_parameter_name)                                                           \
    HAL_StatusTypeDef set##_##function_name##_##on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t input_parameter_name) {      \
        HAL_StatusTypeDef HAL_Status;                                                                                                           \
                                                                                                                                                \
        if (port == MCP23017_PORTA) {                                                                                                           \
            HAL_Status = write_register_bit(hdev, REGISTER_A, pin_number, input_parameter_name);                                                \
        } else if (port == MCP23017_PORTB) {                                                                                                    \
            HAL_Status = write_register_bit(hdev, REGISTER_B, pin_number, input_parameter_name);                                                \
        }                                                                                                                                       \
                                                                                                                                                \
        return HAL_Status;                                                                                                                      \
    }                                                                                                                                           \

#define GET_SET_VALUE_ON_PIN(function_name, REGISTER_A, REGISTER_B, parameter_name)                                                             \
    GET_VALUE_ON_PIN(function_name, REGISTER_A, REGISTER_B, parameter_name)                                                                     \
                                                                                                                                                \
    SET_VALUE_ON_PIN(function_name, REGISTER_A, REGISTER_B, parameter_name)                                                                     \

/// This struct rapresents the MCP23017 device
typedef struct {
    I2C_HandleTypeDef *hi2c; //I2C handle used for communication
    uint8_t device_address; //I2C address of the MCP23017 device
    uint8_t i2c_timeout; //Timeout value for I2C communication in milliseconds
} MCP23017_t;

/// Registers
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

/// Port A or B
#define MCP23017_PORTA    0x00
#define MCP23017_PORTB    0x01

/**
 * @brief Initializes the MCP23017 using the specified configuration.
 *
 * @param hdev Pointer to the MCP23017 device structure.
 * @param hi2c Pointer to the I2C handle used for communication.
 * @param device_address I2C address of the MCP23017 device.
 * @param i2c_timeout Timeout value for I2C communication in milliseconds.
 */
HAL_StatusTypeDef init(MCP23017_t* hdev, I2C_HandleTypeDef* hi2c, uint8_t device_address, uint8_t i2c_timeout);

// Look at 3.5.6 section of the datasheet for details
HAL_StatusTypeDef get_config(MCP23017_t* hdev, uint8_t* config);
HAL_StatusTypeDef set_config(MCP23017_t* hdev, uint8_t config);

/**  
 * You cannot determine the state of IOCON.BANK without hardware resetting it.
 * (https://electronics.stackexchange.com/questions/325916/mcp23017-detecting-state-of-iocon-bank-bit-after-mcu-reset)
 * 
 * I implemented a routine to get it into a known value without resetting it.
 * This function will bring the device always to IOCON.BANK = 0 without modifying anything else.
*/
HAL_StatusTypeDef reset_bank_config_to_default(MCP23017_t* hdev);

/// Look at 3.5.1 section of the datasheet for details
HAL_StatusTypeDef get_io_direction_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* direction);
HAL_StatusTypeDef set_io_direction_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t direction);

/// Look at 3.5.2 section of the datasheet for details
HAL_StatusTypeDef get_input_polarity_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* mode);
HAL_StatusTypeDef set_input_polarity_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t mode);

/// Look at 3.5.3 section of the datasheet for details
HAL_StatusTypeDef get_interrupt_status_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status);

/**
 * IMPORTANT: You need to also set the DEFVAL and INTCON register for this bit
 * 
 * The DEFVAL and INTCON registers must also be configured
 * if any pins are enabled for interrupt-on-change.
 * 
 * DEFVAL -> set_defval_on_pin()
 * INTCON -> set_interrupt_compare_mode_on_pin()
*/
HAL_StatusTypeDef set_interrupt_status_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t status);

/// Look at 3.5.4 section of the datasheet for details
HAL_StatusTypeDef get_defval_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* defval);
HAL_StatusTypeDef set_defval_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t defval);

/// Look at 3.5.5 section of the datasheet for details
HAL_StatusTypeDef get_interrupt_compare_mode_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* mode);
HAL_StatusTypeDef set_interrupt_compare_mode_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t mode);

/// Look at 3.5.7 section of the datasheet for details
HAL_StatusTypeDef get_pull_up_resistor_status_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status);
HAL_StatusTypeDef set_pull_up_resistor_status_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t status);

/**
 * @note Look at 3.5.8 section of the datasheet for details  
 * IMPORTANT: This is a read-only register
*/
HAL_StatusTypeDef get_interrupt_flag_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* value);

/**
 * @note Look at 3.5.9 section of the datasheet for details
 * IMPORTANT: This is a read-only register
 * 
 * The INTCAP register captures the GPIO port value at
 * the time the interrupt occurred. The register is
 * read-only and is updated only when an interrupt
 * occurs. The register remains unchanged until the
 * interrupt is cleared via a read of INTCAP or GPIO.
 * 
 * INTCAP -> get_interrupt_value_on_pin()
 * GPIO   -> get_value_on_pin()
 *
 * @note Interrupt Logic
 * Look at 3.6.1 section of the datasheet for details
 * 
 * There are two interrupt pins: INTA and INTB.
 * By default, INTA is associated with GPAn pins (PORTA)
 * and INTB is associated with GPBn pins (PORTB).
 * Each port has an independent signal which is cleared if
 * its associated GPIO or INTCAP register is read.
 * 
 * IMPORTANT:
 * If INTn pin pins are configured to mirror each other
 * the interrupt will only be cleared if both associated registers are read.
*/
HAL_StatusTypeDef get_interrupt_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* value);

/// Look at 3.5.10 section of the datasheet for details
HAL_StatusTypeDef get_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* value);
HAL_StatusTypeDef read_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t value);

/**
 * @note Advanced
 * To use the following functions you need to know the specification
*/
HAL_StatusTypeDef read_register(MCP23017_t* hdev, uint8_t register_address, uint8_t* register_value);
HAL_StatusTypeDef write_register(MCP23017_t* hdev, uint8_t register_address, uint8_t register_value);
HAL_StatusTypeDef read_register_bit(MCP23017_t* hdev, uint8_t register_address, uint8_t index, uint8_t* register_bit_value);
HAL_StatusTypeDef write_register_bit(MCP23017_t* hdev, uint8_t register_address, uint8_t index, uint8_t register_bit_value);

/**
 * @note Look at 3.5.11 section of the datasheet for details
 * 
 * The OLAT register provides access to the output
 * latches. A read from this register results in a read of the
 * OLAT  and  not  the  port  itself.  A  write  to  this  register
 * modifies  the  output  latches  that  modifies  the  pins
 * configured as outputs.
*/
HAL_StatusTypeDef get_output_latch_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* value);
HAL_StatusTypeDef set_output_latch_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t value);

#endif