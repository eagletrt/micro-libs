/**
 * @file mcp23017.h
 * @version 1.0
 * @date 16/11/2023
 * @author Enrico Dalla Croce (Kalsifer-742)
 * 
 * @brief Driver to configure and operate the MCP23X17 I/O Expander
*/

/**
 * @paragraph Documentation
 * 
 * This file contains declarations and partial documentation for the MCP23017 I/O expander driver.
 * Specific functions are fully documented to serve as examples for similar functions,
 * assuming a consistent behavior across the driver.
 * Specific functions should be thoroughly documented, outlining any unique behaviors.
 * For comprehensive details, consult the MCP23017 datasheet.
*/

#ifndef mcp_23017_h
#define mcp_23017_h

/// Includes
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @struct MCP23017_t
 * @brief rapresents the MCP23017 device
 */
typedef struct {
    I2C_HandleTypeDef *hi2c; ///I2C handle used for communication.
    uint8_t device_address; ///I2C address of the MCP23017 device.
    uint8_t i2c_timeout; ///Timeout value for I2C communication in milliseconds
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
 * @paragraph Simple
 * 
 * The following function are for simplified use of the device.
*/

/**
 * @brief Initializes the MCP23017 using the specified configuration.
 *
 * @param hdev Pointer to the MCP23017 device structure.
 * @param hi2c Pointer to the I2C handle used for communication.
 * @param device_address I2C address of the MCP23017 device.
 * @param i2c_timeout Timeout value for I2C communication in milliseconds.
 */
HAL_StatusTypeDef init(MCP23017_t* hdev, I2C_HandleTypeDef* hi2c, uint8_t device_address, uint8_t i2c_timeout);

/**  
 * You cannot determine the state of IOCON.BANK without hardware resetting it.
 * (https://electronics.stackexchange.com/questions/325916/mcp23017-detecting-state-of-iocon-bank-bit-after-mcu-reset)
 * 
 * I implemented a routine to get it into a known value without resetting it.
 * This function will bring the device always to IOCON.BANK = 0 without modifying anything else.
*/
HAL_StatusTypeDef reset_bank_config_to_default(MCP23017_t* hdev);

/**
 * @brief Look at 3.5.1 section of the datasheet for details
*/
HAL_StatusTypeDef get_io_direction_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* direction);
HAL_StatusTypeDef set_io_direction_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t direction);

/**
 * @brief Look at 3.5.2 section of the datasheet for details
*/
HAL_StatusTypeDef get_input_polarity_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* mode);
HAL_StatusTypeDef set_input_polarity_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t mode);

/**
 * @brief Look at 3.5.3 section of the datasheet for details
*/
HAL_StatusTypeDef get_interrupt_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status);

/**
 * @brief You need to also set the DEFVAL and INTCON register for this bit
 * 
 * The DEFVAL and INTCON registers must also be configured
 * if any pins are enabled for interrupt-on-change.
 * 
 * DEFVAL -> set_defval_on_pin()
 * INTCON -> set_interrupt_compare_mode_on_pin()
*/
HAL_StatusTypeDef set_interrupt_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t status);

/**
 * @brief Look at 3.5.4 section of the datasheet for details
*/
HAL_StatusTypeDef get_defval_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* defval);
HAL_StatusTypeDef set_defval_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t defval);

/**
 * @brief Look at 3.5.5 section of the datasheet for details
*/
HAL_StatusTypeDef get_interrupt_compare_mode_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* mode);
HAL_StatusTypeDef set_interrupt_compare_mode_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t mode);

/**
 * @brief Look at 3.5.7 section of the datasheet for details
*/
HAL_StatusTypeDef get_pull_up_resistor_status_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status);
HAL_StatusTypeDef set_pull_up_resistor_status_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t status);

/**
 * @brief Look at 3.5.8 section of the datasheet for details
 * 
 * This is a read-only register
*/
HAL_StatusTypeDef get_interrupt_flag_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status);

/**
 * @brief Look at 3.5.9 section of the datasheet for details
 * 
 * The INTCAP register captures the GPIO port value at
 * the time the interrupt occurred. The register is
 * read-only and is updated only when an interrupt
 * occurs. The register remains unchanged until the
 * interrupt is cleared via a read of INTCAP or GPIO.
 * 
 * INTCAP -> get_interrupt_value_on_pin()
 * GPIO   -> read_value_on_pin()
*/
HAL_StatusTypeDef get_interrupt_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status);

/**
 * @paragraph Interrupt Logic
 * 
 * Section 3.6.1
 * There are two interrupt pins: INTA and INTB.
 * Bydefault,INTA is associated with GPAn pins (PORTA)
 * and INTB is associated with GPBn pins (PORTB).
 * Each port has an independent signal which is cleared if
 * its associated GPIO or INTCAP register is read.
 * 
 * IMPORTANT:
 * If INTn pin pins are configured to mirror each other
 * the interrupt will only be cleared if both associated registers are read.
*/

/**
 * @brief Read the value on a specific pin of the MCP23017 device.
 *
 * This function reads the value (high or low) on the specified pin
 * of the MCP23017 device.
 * 
 * Look at 3.5.10 section of the datasheet for details
 *
 * @param hdev Pointer to the MCP23017 device structure.
 * @param port Specifies the port (PORTA or PORTB) to which the pin belongs.
 * @param pin_number The pin number for which to read the logic level (0 to 7).
 * @param status Pointer to the variable where the read logic level will be stored.
 *
 * @return HAL_StatusTypeDef
 *   - HAL_OK: reading successful, and the result is stored in the 'status' parameter.
 *   - HAL_ERROR: An error occurred during the reading.
 *   - HAL_BUSY: The MCP23017 device is busy, and the operation cannot be performed.
 *   - HAL_TIMEOUT: The operation timed out while communicating with the MCP23017 device.
 *
 * @note The 'status' parameter will be populated with either 1 or 0,
 *       indicating the current logic level on the specified pin.
 */
HAL_StatusTypeDef read_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status);

/**
 * @brief Write a value to a specific pin of the MCP23017 device.
 *
 * This function sets value (high or low) on the specified pin of
 * the MCP23017 device.
 * 
 * Look at 3.5.10 section of the datasheet for details
 *
 * @param hdev Pointer to the MCP23017 device structure.
 * @param port Specifies the port (PORTA or PORTB) to which the pin belongs.
 * @param pin_number The pin number for which to set the logic level (0 to 7).
 * @param value Desired value to be set on the specified pin (0 or 1).
 *
 * @return HAL_StatusTypeDef
 *   - HAL_OK: value writing successful.
 *   - HAL_ERROR: An error occurred during the value writing.
 *   - HAL_BUSY: The MCP23017 device is busy, and the operation cannot be performed.
 *   - HAL_TIMEOUT: The operation timed out while communicating with the MCP23017 device.
 */
HAL_StatusTypeDef write_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t value);

/**
 * @paragraph Advanced
 * 
 * The following function are for advanced use of the device.
 * You need to know the specifications to use this functions correctly.
*/

HAL_StatusTypeDef read_register(MCP23017_t* hdev, uint8_t register_address, uint8_t* register_value);
HAL_StatusTypeDef write_register(MCP23017_t* hdev, uint8_t register_address, uint8_t register_value);
HAL_StatusTypeDef read_register_bit(MCP23017_t* hdev, uint8_t register_address, uint8_t index, uint8_t* register_bit_value);
HAL_StatusTypeDef write_register_bit(MCP23017_t* hdev, uint8_t register_address, uint8_t index, uint8_t register_bit_value);

/**
 * @brief Look at 3.5.11 section of the datasheet for details
 * 
 * The OLAT register provides access to the output
 * latches. A read from this register results in a read of the
 * OLAT  and  not  the  port  itself.  A  write  to  this  register
 * modifies  the  output  latches  that  modifies  the  pins
 * configured as outputs.
*/
HAL_StatusTypeDef get_output_latch_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status);
HAL_StatusTypeDef set_output_latch_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t status);

///Include guards
#endif