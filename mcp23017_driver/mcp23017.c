#include "mcp23017.h"

typedef struct {
    uint8_t IODIRA,
    IODIRB,
    IPOLA,
    IPOLB,
    GPINTENA,
    GPINTENB,
    DEFVALA,
    DEFVALB,
    INTCONA,
    INTCONB,
    IOCON,
    GPPUA,
    GPPUB,
    INTFA,
    INTFB,
    INTCAPA,
    INTCAPB,
    _GPIOA,
    _GPIOB,
    OLATA,
    OLATB;
} Registers_t;

Registers_t bank0_registers_addresses = {
    .IODIRA   = 0x00,
    .IODIRB   = 0x01,
    .IPOLA    = 0x02,
    .IPOLB    = 0x03,
    .GPINTENA = 0x04,
    .GPINTENB = 0x05, 
    .DEFVALA  = 0x06,
    .DEFVALB  = 0x07,
    .INTCONA  = 0x08,
    .INTCONB  = 0x09,
    .IOCON    = 0x0A,
    /*
    .IOCONA = 0x0A,
    .IOCONB = 0x0B,
    */
    .GPPUA    = 0x0C,
    .GPPUB    = 0x0D,
    .INTFA    = 0x0E,
    .INTFB    = 0x0F,
    .INTCAPA  = 0x10,
    .INTCAPB  = 0x11,
    ._GPIOA   = 0x12,
    ._GPIOB   = 0x13,
    .OLATA    = 0x14,
    .OLATB    = 0x15
};

Registers_t bank1_registers_addresses = {
    .IODIRA   = 0x00,
    .IPOLA    = 0x01,
    .GPINTENA = 0x02,
    .DEFVALA  = 0x03,
    .INTCONA  = 0x04,
    .IOCON    = 0x05,  
    .GPPUA    = 0x06,
    .INTFA    = 0x07,
    .INTCAPA  = 0x08,
    ._GPIOA   = 0x09,
    .OLATA    = 0x0A,
    .IODIRB   = 0x10,
    .IPOLB    = 0x11,
    .GPINTENB = 0x12,
    .DEFVALB  = 0x13,
    .INTCONB  = 0x14,
    .IOCON    = 0x15,
    .GPPUB    = 0x16,
    .INTFB    = 0x17,
    .INTCAPB  = 0x18,
    ._GPIOB   = 0x19,
    .OLATB    = 0x1A
};

Registers_t current_registers_addresses;

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

HAL_StatusTypeDef get_config(MCP23017_t* hdev, MCP23017_configuration_t* config) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t register_value;

    HAL_Status = read_register(hdev, current_registers_addresses.IOCON, &register_value);
    if (HAL_Status != HAL_OK) {
        return HAL_Status;
    }

    config->bank = get_register_bit(register_value, 7);
    config->mirror = get_register_bit(register_value, 6);
    config->seqop = get_register_bit(register_value, 5);
    config->disslw = get_register_bit(register_value, 4);
    //config->haen_MCP23S17_only = get_register_bit(register_value, 3);
    config->odr = get_register_bit(register_value, 2);
    config->intpol = get_register_bit(register_value, 1);
    //get_register_bit(register_value, 0);

    return HAL_Status;
}

HAL_StatusTypeDef set_config(MCP23017_t* hdev, MCP23017_configuration_t config) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t register_value;

    set_register_bit(&register_value, 7, config.bank);
    set_register_bit(&register_value, 6, config.mirror);
    set_register_bit(&register_value, 5, config.seqop);
    set_register_bit(&register_value, 4, config.disslw);
    //set_register_bit(&register_value, 3, config.haen_MCP23S17_only);
    set_register_bit(&register_value, 2, config.odr);
    set_register_bit(&register_value, 1, config.intpol);
    //set_register_bit(register_value, 0, 0);

    if (config.bank == 1) {
        //unnecessary, but just to be sure
        current_registers_addresses = bank0_registers_addresses;

        HAL_Status = write_register_bit(hdev, current_registers_addresses.IOCON, 7, 1);
        if (HAL_Status != HAL_OK) {
            return HAL_Status;
        }

        current_registers_addresses = bank1_registers_addresses;
    } else if (config.bank == 0) {
        //unnecessary, but just to be sure
        current_registers_addresses = bank1_registers_addresses;

        HAL_Status = write_register_bit(hdev, current_registers_addresses.IOCON, 7, 0);
        if (HAL_Status != HAL_OK) {
            return HAL_Status;
        }

        current_registers_addresses = bank0_registers_addresses;
    }

    HAL_Status = write_register(hdev, current_registers_addresses.IOCON, register_value);

    return HAL_Status;
}

HAL_StatusTypeDef reset_config(MCP23017_t* hdev) {
    HAL_StatusTypeDef HAL_Status;

    HAL_Status = set_config(hdev, default_configuration);

    return HAL_Status;
}

HAL_StatusTypeDef init(MCP23017_t* hdev, I2C_HandleTypeDef* hi2c, uint8_t device_address, uint8_t i2c_timeout, MCP23017_configuration_t config) {
    HAL_StatusTypeDef HAL_Status;

    hdev->hi2c = hi2c;
    hdev->device_address = device_address;
    hdev->i2c_timeout = i2c_timeout;
    HAL_Status = set_config(hdev, config);

    return HAL_Status;
}

HAL_StatusTypeDef reset_bank_config(MCP23017_t* hdev) {
    HAL_StatusTypeDef HAL_Status;
    //Assume IOCON.BANK = 1
    uint8_t IOCON_address = 0x05;
    uint8_t register_value;
    uint8_t old_bit_value;

    //Read register 0x05
    HAL_Status = read_register(hdev, IOCON_address, &register_value);
    if (HAL_Status != HAL_OK) {
        return HAL_Status;
    }

    //Save old value
    old_bit_value = get_register_bit(register_value, 7);

    //Clear bit 7 (BANK bit)
    set_register_bit(&register_value, 7, 0);
    HAL_Status = write_register(hdev, IOCON_address, register_value);
    if (HAL_Status != HAL_OK) {
        return HAL_Status;
    }

    //Save new state
    current_registers_addresses = bank0_registers_addresses;

    /**
     * At this point you have either:
     * Switched from BANK = 1 to BANK = 0
     * or
     * Disabled GPINTENB.GPINT7
    */

    //Restore GPINTENB.GPINT7
    HAL_Status = write_register_bit(hdev, current_registers_addresses.GPINTENB, 7, old_bit_value);
    
    //Now you are in known state IOCON.BANK = 0
    
    return HAL_Status;
}

HAL_StatusTypeDef get_io_direction_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, io_direction_t* direction) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;

    if (port == port_a) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.IODIRA, pin_number, &bit_value);
    } else if (port == port_b) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.IODIRB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        if (bit_value == 1) {
            *direction = io_direction_input;
        } else if (bit_value == 0) {
            *direction = io_direction_output;
        }
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_io_direction_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, io_direction_t direction) {
    HAL_StatusTypeDef HAL_Status;

    if (port == port_a) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.IODIRA, pin_number, direction);
    } else if (port == port_b) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.IODIRB, pin_number, direction);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_input_polarity_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, input_polarity_mode_t* mode) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;

    if (port == port_a) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.IPOLA, pin_number, &bit_value);
    } else if (port == port_b) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.IPOLB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        if (bit_value == 1) {
            *mode = input_polarity_inverted;
        } else if (bit_value == 0) {
            *mode = input_polarity_normal;
        }
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_input_polarity_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, input_polarity_mode_t mode) {
    HAL_StatusTypeDef HAL_Status;

    if (port == port_a) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.IPOLA, pin_number, mode);
    } else if (port == port_b) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.IPOLB, pin_number, mode);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_interrupt_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_status_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;

    if (port == port_a) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.GPINTENA, pin_number, &bit_value);
    } else if (port == port_b) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.GPINTENB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        if (bit_value == 1) {
            *status = interrupt_enabled;
        } else if (bit_value == 0) {
            *status = interrupt_disabled;
        }
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_interrupt_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_status_t status) {
    HAL_StatusTypeDef HAL_Status;

    if (port == port_a) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.GPINTENA, pin_number, status);
    } else if (port == port_b) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.GPINTENB, pin_number, status);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_defval_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* defval) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == port_a) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.DEFVALA, pin_number, &bit_value);
    } else if (port == port_b) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.DEFVALB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        if (bit_value == 1) {
            *defval = logic_high;
        } else if (bit_value == 0) {
            *defval = logic_low;
        }
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_defval_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t defval) {
    HAL_StatusTypeDef HAL_Status;

    if (port == port_a) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.DEFVALA, pin_number, defval);
    } else if (port == port_b) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.DEFVALB, pin_number, defval);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_interrupt_compare_mode_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_compare_mode_t* mode) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == port_a) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.INTCONA, pin_number, &bit_value);
    } else if (port == port_b) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.INTCONB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        if (bit_value == 1) {
            *mode = interrupt_compare_with_ref_value;
        } else if (bit_value == 0) {
            *mode = interrupt_compare_with_prev_value;
        }
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_interrupt_compare_mode_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, interrupt_compare_mode_t mode) {
    HAL_StatusTypeDef HAL_Status;
    
    if (port == port_a) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.INTCONA, pin_number, mode);
    } else if (port == port_b) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.INTCONB, pin_number, mode);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_pull_up_resistor_status_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, pull_up_resistor_status_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == port_a) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.INTCONA, pin_number, &bit_value);
    } else if (port == port_b) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.INTCONB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        if (bit_value == 1) {
            *status = pull_up_resistor_enabled;
        } else if (bit_value == 0) {
            *status = pull_up_resistor_disable;
        }
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_pull_up_resistor_status_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, pull_up_resistor_status_t status) {
    HAL_StatusTypeDef HAL_Status;
    
    if (port == port_a) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.INTCONA, pin_number, status);
    } else if (port == port_b) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.INTCONB, pin_number, status);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_interrupt_flag_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, flag_status_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == port_a) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.INTFA, pin_number, &bit_value);
    } else if (port == port_b) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.INTFB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        if (bit_value == 1) {
            *status = flag_high;
        } else if (bit_value == 0) {
            *status = flag_low;
        }
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_interrupt_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == port_a) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.INTCAPA, pin_number, &bit_value);
    } else if (port == port_b) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.INTCAPB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        if (bit_value == 1) {
            *status = logic_high;
        } else if (bit_value == 0) {
            *status = logic_low;
        }
    }

    return HAL_Status;
}

HAL_StatusTypeDef read_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == port_a) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses._GPIOA, pin_number, &bit_value);
    } else if (port == port_b) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses._GPIOB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        if (bit_value == 1) {
            *status = logic_high;
        } else if (bit_value == 0) {
            *status = logic_low;
        }
    }

    return HAL_Status;
}

HAL_StatusTypeDef write_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t value) {
    HAL_StatusTypeDef HAL_Status;
    
    if (port == port_a) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.GPPUA, pin_number, value);
    } else if (port == port_b) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.GPPUB, pin_number, value);
    }

    return HAL_Status;
}

HAL_StatusTypeDef get_output_latch_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t* status) {
    HAL_StatusTypeDef HAL_Status;
    uint8_t bit_value;
    
    if (port == port_a) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.INTCONA, pin_number, &bit_value);
    } else if (port == port_b) {
        HAL_Status = read_register_bit(hdev, current_registers_addresses.INTCONB, pin_number, &bit_value);
    }

    if (HAL_Status == HAL_OK) {
        if (bit_value == 1) {
            *status = logic_high;
        } else if (bit_value == 0) {
            *status = logic_low;
        }
    }

    return HAL_Status;
}

HAL_StatusTypeDef set_output_latch_value_on_pin(MCP23017_t* hdev, port_t port, uint8_t pin_number, logic_pin_level_t status) {
    HAL_StatusTypeDef HAL_Status;
    
    if (port == port_a) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.INTCONA, pin_number, status);
    } else if (port == port_b) {
        HAL_Status = write_register_bit(hdev, current_registers_addresses.INTCONB, pin_number, status);
    }

    return HAL_Status;
}