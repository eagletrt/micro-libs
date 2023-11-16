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

HAL_StatusTypeDef init(MCP23017_t* hdev, I2C_HandleTypeDef* hi2c, uint8_t device_address, uint8_t i2c_timeout) {
    HAL_StatusTypeDef HAL_Status;

    hdev->hi2c = hi2c;
    hdev->device_address = device_address;
    hdev->i2c_timeout = i2c_timeout;

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

HAL_StatusTypeDef get_io_direction_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* direction) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;

    if (port == MCP23017_PORTA) {
        HAL_Status = read_register_bit(hdev, REGISTER_GPIOA, pin_number, &bit_value);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = read_register_bit(hdev, REGISTER_GPIOB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        *direction = bit_value;
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_io_direction_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t direction) {
    HAL_StatusTypeDef HAL_Status;

    if (port == MCP23017_PORTA) {
        HAL_Status = write_register_bit(hdev, REGISTER_IODIRA, pin_number, direction);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = write_register_bit(hdev, REGISTER_IODIRB, pin_number, direction);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_input_polarity_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* mode) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;

    if (port == MCP23017_PORTA) {
        HAL_Status = read_register_bit(hdev, REGISTER_IPOLA, pin_number, &bit_value);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = read_register_bit(hdev, REGISTER_IPOLB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        *mode = bit_value;
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_input_polarity_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t mode) {
    HAL_StatusTypeDef HAL_Status;

    if (port == MCP23017_PORTA) {
        HAL_Status = write_register_bit(hdev, REGISTER_IPOLA, pin_number, mode);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = write_register_bit(hdev, REGISTER_IPOLB, pin_number, mode);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_interrupt_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;

    if (port == MCP23017_PORTA) {
        HAL_Status = read_register_bit(hdev, REGISTER_GPINTENA, pin_number, &bit_value);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = read_register_bit(hdev, REGISTER_GPINTENB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        *status = bit_value;
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_interrupt_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t status) {
    HAL_StatusTypeDef HAL_Status;

    if (port == MCP23017_PORTA) {
        HAL_Status = write_register_bit(hdev, REGISTER_GPINTENA, pin_number, status);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = write_register_bit(hdev, REGISTER_GPINTENB, pin_number, status);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_defval_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* defval) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == MCP23017_PORTA) {
        HAL_Status = read_register_bit(hdev, REGISTER_DEFVALA, pin_number, &bit_value);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = read_register_bit(hdev, REGISTER_DEFVALB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        *defval = bit_value;
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_defval_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t defval) {
    HAL_StatusTypeDef HAL_Status;

    if (port == MCP23017_PORTA) {
        HAL_Status = write_register_bit(hdev, REGISTER_DEFVALA, pin_number, defval);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = write_register_bit(hdev, REGISTER_DEFVALB, pin_number, defval);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_interrupt_compare_mode_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* mode) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == MCP23017_PORTA) {
        HAL_Status = read_register_bit(hdev, REGISTER_INTCONA, pin_number, &bit_value);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = read_register_bit(hdev, REGISTER_INTCONB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        *mode = bit_value;
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_interrupt_compare_mode_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t mode) {
    HAL_StatusTypeDef HAL_Status;
    
    if (port == MCP23017_PORTA) {
        HAL_Status = write_register_bit(hdev, REGISTER_INTCONA, pin_number, mode);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = write_register_bit(hdev, REGISTER_INTCONB, pin_number, mode);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_pull_up_resistor_status_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == MCP23017_PORTA) {
        HAL_Status = read_register_bit(hdev, REGISTER_INTCONA, pin_number, &bit_value);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = read_register_bit(hdev, REGISTER_INTCONB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        *status = bit_value;
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_pull_up_resistor_status_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t status) {
    HAL_StatusTypeDef HAL_Status;
    
    if (port == MCP23017_PORTA) {
        HAL_Status = write_register_bit(hdev, REGISTER_INTCONA, pin_number, status);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = write_register_bit(hdev, REGISTER_INTCONB, pin_number, status);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_interrupt_flag_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == MCP23017_PORTA) {
        HAL_Status = read_register_bit(hdev, REGISTER_INTFA, pin_number, &bit_value);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = read_register_bit(hdev, REGISTER_INTFB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        *status = bit_value;
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_interrupt_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == MCP23017_PORTA) {
        HAL_Status = read_register_bit(hdev, REGISTER_INTCAPA, pin_number, &bit_value);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = read_register_bit(hdev, REGISTER_INTCAPB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        *status = bit_value;
    }

    return HAL_Status;
}

HAL_StatusTypeDef read_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == MCP23017_PORTA) {
        HAL_Status = read_register_bit(hdev, REGISTER_GPIOA, pin_number, &bit_value);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = read_register_bit(hdev, REGISTER_GPIOB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        *status = bit_value;
    }

    return HAL_Status;
}

HAL_StatusTypeDef write_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t value) {
    HAL_StatusTypeDef HAL_Status;
    
    if (port == MCP23017_PORTA) {
        HAL_Status = write_register_bit(hdev, REGISTER_GPPUA, pin_number, value);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = write_register_bit(hdev, REGISTER_GPPUB, pin_number, value);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_output_latch_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == MCP23017_PORTA) {
        HAL_Status = read_register_bit(hdev, REGISTER_INTCONA, pin_number, &bit_value);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = read_register_bit(hdev, REGISTER_INTCONB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        *status = bit_value;
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_output_latch_value_on_pin(MCP23017_t* hdev, uint8_t port, uint8_t pin_number, uint8_t status) {
    HAL_StatusTypeDef HAL_Status;
    
    if (port == MCP23017_PORTA) {
        HAL_Status = write_register_bit(hdev, REGISTER_INTCONA, pin_number, status);
    } else if (port == MCP23017_PORTB) {
        HAL_Status = write_register_bit(hdev, REGISTER_INTCONB, pin_number, status);
    }

    return HAL_Status;
}