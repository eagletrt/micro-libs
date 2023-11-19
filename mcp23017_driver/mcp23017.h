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

/**
 * @struct MCP23017_t
 * @brief rapresents the configuration register (IOCON)
*/
typedef struct {
    /**
     * @brief Controls how the registers are addressed
     * 1 = The registers associated with each port are separated into different banks.
     * 0 = The registers are in the same bank (addresses are sequential).
     */
    uint8_t bank,
    /**
     * @brief INT Pins Mirror bit
     * 1 = The INT pins are internally connected
     * 0 = The INT pins are not connected. INTA is associated with PORTA and INTB is associated with PORTB
     */
    mirror,
    /**
     * @brief Sequential Operation mode bit
     * 1 = Sequential operation disabled, address pointer does not increment.
     * 0 = Sequential operation enabled, address pointer increments
     */
    seqop,
    /**
     * @brief Slew Rate control bit for SDA output
     * 1 = Slew rate disabled
     * 0 = Slew rate enabled
     */
    disslw,
    /**
     * @brief Address pins are always enabled on the MCP23017. this setting is only for the MCP23S17.
     */
    haen_MCP23S17_only,
    /**
     * @brief Configures the INT pin as an open-drain output
     * 1 = Open-drain output (overrides the INTPOL bit.)
     * 0 = Active driver output (INTPOL bit sets the polarity.)
     */
    odr,
    /**
     * @brief This bit sets the polarity of the INT output pin
     * 1 = Active-high
     * 0 = Active-low
     */
    intpol;
} MCP23017_configuration_t;

MCP23017_configuration_t default_configuration = {
    .bank   = 0,
    .mirror = 0, 
    .seqop  = 0,
    .disslw = 0,
    .odr    = 0,
    .intpol = 0
};

/**
 * @struct MCP23017_t
 * @brief rapresents the MCP23017 device
 */
typedef struct {
    I2C_HandleTypeDef *hi2c; ///I2C handle used for communication.
    uint8_t device_address; ///I2C address of the MCP23017 device.
    uint8_t i2c_timeout; ///Timeout value for I2C communication in milliseconds
} MCP23017_t;

/**
 * @typedef
 * @brief Specifies whether to refer to the PORTA or PORTB register.
 */
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

/**
 * @paragraph Simple
 * 
 * The following function are for simplified use of the device.
*/

/**
 * @brief Get current configuration from device.
 * @param hdev Pointer to the MCP23017 device structure.
 * @param config Pointer to the MCP23017 configuration structure.
 * 
 * @return HAL status from communication with the device via I2C.
 */
HAL_StatusTypeDef get_config(MCP23017_t* hdev, MCP23017_configuration_t* config);

/**
 * @brief Set configuration to device.
 */
HAL_StatusTypeDef set_config(MCP23017_t* hdev, MCP23017_configuration_t config);

/**
 * @brief reset configuration to the default. refer to the default_configuration struct. 
*/
HAL_StatusTypeDef reset_config(MCP23017_t* hdev);

/**
 * @brief Initializes the MCP23017 using the specified configuration.
 *
 * @param hdev Pointer to the MCP23017 device structure.
 * @param hi2c Pointer to the I2C handle used for communication.
 * @param device_address I2C address of the MCP23017 device.
 * @param i2c_timeout Timeout value for I2C communication in milliseconds.
 * @param config Configuration settings for the MCP23017.
 * 
 * @return HAL status from communication with the device via I2C.
 */
HAL_StatusTypeDef init(MCP23017_t* hdev, I2C_HandleTypeDef* hi2c, uint8_t device_address, uint8_t i2c_timeout, MCP23017_configuration_t config);

/**
 * @brief
 * 
 * https://electronics.stackexchange.com/questions/325916/mcp23017-detecting-state-of-iocon-bank-bit-after-mcu-reset
 * 
 * You cannot determine the state of IOCON.BANK without hardware resetting it.
 * I implemented a routine to get it into a known value without resetting it.
 * This function will bring the device always to IOCON.BANK = 0 without modifying anything else.
 * 
*/
HAL_StatusTypeDef reset_bank_config(MCP23017_t* hdev);

/**
 * @brief Look at 3.5.1 section of the datasheet for details
*/
HAL_StatusTypeDef get_io_direction_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, io_direction_t* direction);
HAL_StatusTypeDef set_io_direction_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, io_direction_t direction);

/**
 * @brief Look at 3.5.2 section of the datasheet for details
*/
HAL_StatusTypeDef get_input_polarity_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, input_polarity_mode_t* mode);
HAL_StatusTypeDef set_input_polarity_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, input_polarity_mode_t mode);

/**
 * @brief Look at 3.5.3 section of the datasheet for details
*/
HAL_StatusTypeDef get_interrupt_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_status_t* status);

/**
 * @brief You need to also set the DEFVAL and INTCON register for this bit
 * 
 * The DEFVAL and INTCON registers  must  also  be  configured
 * if  any  pins  are enabled for interrupt-on-change.
 * 
 * DEFVAL -> set_defval_on_pin()
 * INTCON -> set_interrupt_compare_mode_on_pin()
*/
HAL_StatusTypeDef set_interrupt_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_status_t status);

/**
 * @brief Look at 3.5.4 section of the datasheet for details
*/
HAL_StatusTypeDef get_defval_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* defval);
HAL_StatusTypeDef set_defval_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t defval);

/**
 * @brief Look at 3.5.5 section of the datasheet for details
*/
HAL_StatusTypeDef get_interrupt_compare_mode_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_compare_mode_t* mode);
HAL_StatusTypeDef set_interrupt_compare_mode_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_compare_mode_t mode);

/**
 * @brief Look at 3.5.7 section of the datasheet for details
*/
HAL_StatusTypeDef get_pull_up_resistor_status_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, pull_up_resistor_status_t* status);
HAL_StatusTypeDef set_pull_up_resistor_status_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, pull_up_resistor_status_t status);

/**
 * @brief Look at 3.5.8 section of the datasheet for details
 * 
 * This is a read-only register
*/
HAL_StatusTypeDef get_interrupt_flag_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, flag_status_t* status);

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
HAL_StatusTypeDef get_interrupt_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* status);

/**
 * @paragraph Interrupt Logic
 * 
 * Section 3.6.1
 * There  are  two  interrupt  pins:  INTA  and  INTB.
 * Bydefault, INTA is associated with GPAn pins (PORTA)
 * and INTB is associated with GPBn pins (PORTB).
 * Each port has an independent signal which is cleared if
 * its associated GPIO or INTCAP register is read.
 * 
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
 * @note The 'status' parameter will be populated with either logic_high or logic_low,
 *       of the logic_pin_level_t type, indicating the current logic level on the specified pin.
 */
HAL_StatusTypeDef read_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* status);

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
 * @param value Desired value to be set on the specified pin (logic_high or logic_low).
 *
 * @return HAL_StatusTypeDef
 *   - HAL_OK: value writing successful.
 *   - HAL_ERROR: An error occurred during the value writing.
 *   - HAL_BUSY: The MCP23017 device is busy, and the operation cannot be performed.
 *   - HAL_TIMEOUT: The operation timed out while communicating with the MCP23017 device.
 *
 * @note Use logic_low to set the pin to a low logic level and logic_high
 *       to set the pin to a high logic level.
 */
HAL_StatusTypeDef write_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t value);

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
HAL_StatusTypeDef get_output_latch_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* status);
HAL_StatusTypeDef set_output_latch_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t status);

///Include guards
#endif