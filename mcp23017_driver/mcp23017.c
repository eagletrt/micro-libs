/**
 * @author Enrico Dalla Croce (Kalsifer-742)
 * @date 30/11/2023
*/ 

#include "mcp23017.h"

uint8_t get_register_bit(uint8_t register_value, uint8_t index) {
    uint8_t bit_value = (register_value >> index) & 1;

    return bit_value;
}

void set_register_bit(uint8_t* register_value, uint8_t index, uint8_t bit_value) {
    if (bit_value == 0) {
        *register_value = *register_value & ~(1 << index);
    } else if (bit_value == 1) {
        *register_value = *register_value | (1 << index);
    }
}

HAL_StatusTypeDef read_register(MCP23017_t* hdev, uint8_t register_address, uint8_t* register_value) {
    HAL_StatusTypeDef HAL_Status;

    HAL_Status = HAL_I2C_Mem_Read(
        hdev->hi2c, 
        hdev->device_address, 
        register_address,
        I2C_MEMADD_SIZE_8BIT, 
        register_value, 
        1, 
        hdev->i2c_timeout
    );

    return HAL_Status;
}

HAL_StatusTypeDef write_register(MCP23017_t* hdev, uint8_t register_address, uint8_t register_value) {  
    HAL_StatusTypeDef HAL_Status;
    
    HAL_Status = HAL_I2C_Mem_Write(
        hdev->hi2c,
        hdev->device_address,
        register_address,
        I2C_MEMADD_SIZE_8BIT,
        &register_value,
        1,
        hdev->i2c_timeout
    );

    return HAL_Status;
}

HAL_StatusTypeDef read_register_bit(MCP23017_t* hdev, uint8_t register_address, uint8_t index, uint8_t* register_bit_value) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t register_value;
    
    HAL_Status = read_register(hdev, register_address, &register_value);
    *register_bit_value = get_register_bit(register_value, index);

    return HAL_Status;
}

HAL_StatusTypeDef write_register_bit(MCP23017_t* hdev, uint8_t register_address, uint8_t index, uint8_t register_bit_value) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t register_value;

    HAL_Status = read_register(hdev, register_address, &register_value);
    if (HAL_Status != HAL_OK) {
        return HAL_Status;
    }

    set_register_bit(&register_value, index, register_bit_value);
    HAL_Status = write_register(hdev, register_address, register_value);

    return HAL_Status;
}

HAL_StatusTypeDef init(MCP23017_t* hdev, I2C_HandleTypeDef* hi2c, uint8_t device_address, uint8_t i2c_timeout) {
    HAL_StatusTypeDef HAL_Status;

    hdev->hi2c = hi2c;
    hdev->device_address = device_address;
    hdev->i2c_timeout = i2c_timeout;

    return HAL_Status;
}

HAL_StatusTypeDef get_config(MCP23017_t* hdev, uint8_t* config) {
    HAL_StatusTypeDef HAL_Status;

    HAL_Status = read_register(hdev, REGISTER_IOCON, &config);
    if (HAL_Status != HAL_OK) {
        return HAL_Status;
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_config(MCP23017_t* hdev, uint8_t config) {
    HAL_StatusTypeDef HAL_Status;

    //This is to ensure that the bank setting remains at its default value (BANK = 0)
    set_register_bit(&config, 7, 0);

    HAL_Status = write_register(hdev, REGISTER_IOCON, config);

    return HAL_Status;
}

HAL_StatusTypeDef reset_bank_config(MCP23017_t* hdev) {
    HAL_StatusTypeDef HAL_Status;
    // Assume IOCON.BANK = 1
    uint8_t IOCON_address = 0x05;
    uint8_t register_value;
    uint8_t old_bit_value;

    // Read register 0x05
    HAL_Status = read_register(hdev, IOCON_address, &register_value);
    if (HAL_Status != HAL_OK) {
        return HAL_Status;
    }

    // Save old value
    old_bit_value = get_register_bit(register_value, 7);

    // Clear bit 7 (BANK bit)
    set_register_bit(&register_value, 7, 0);
    HAL_Status = write_register(hdev, IOCON_address, register_value);
    if (HAL_Status != HAL_OK) {
        return HAL_Status;
    }

    /**
     * At this point you have either:
     * Switched from BANK = 1 to BANK = 0
     * or
     * Disabled GPINTENB.GPINT7
    */

    //Restore GPINTENB.GPINT7
    HAL_Status = write_register_bit(hdev, REGISTER_GPINTENB, 7, old_bit_value);
    
    //Now you are in known state IOCON.BANK = 0
    
    return HAL_Status;
}

GET_SET_VALUE_ON_PIN(io_direction, REGISTER_IODIRA, REGISTER_IODIRB, direction)

GET_SET_VALUE_ON_PIN(input_polarity, REGISTER_IPOLA, REGISTER_IPOLB, mode)

GET_SET_VALUE_ON_PIN(interrupt_status, REGISTER_GPINTENA, REGISTER_GPINTENB, status)

GET_SET_VALUE_ON_PIN(defval, REGISTER_DEFVALA, REGISTER_DEFVALB, defval)

GET_SET_VALUE_ON_PIN(interrupt_compare_mode, REGISTER_INTCONA, REGISTER_INTCONB, mode)

GET_SET_VALUE_ON_PIN(pull_up_resistor_status, REGISTER_GPPUA, REGISTER_GPPUB, status)

GET_VALUE_ON_PIN(interrupt_flag, REGISTER_INTFA, REGISTER_INTFB, status)

GET_VALUE_ON_PIN(interrupt_value, REGISTER_INTCAPA, REGISTER_INTCAPB, status)

GET_SET_VALUE_ON_PIN(value, REGISTER_GPIOA, REGISTER_GPIOB, value)

GET_SET_VALUE_ON_PIN(output_latch_value, REGISTER_OLATA, REGISTER_OLATB, status)