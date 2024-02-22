# MCP23017 I/O Expander - Driver

## Documentation

- [Datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf)

This driver provides functions to:
- [x] Read registers
- [x] Write registers
- [x] Configure interrupts on pins

Documentation can be found in the file `mcp23017.h` as comments. See also the examples below.

### Interrupt

#### Get interrupt info

The INTCAP register captures the GPIO port value at
the time the interrupt occurred. The register is
read-only and is updated only when an interrupt
occurs. The register remains unchanged until the
interrupt is cleared via a read of INTCAP or GPIO.

> Look at section 3.5.9 of the datasheet for details

#### Interrupt Logic

There are two interrupt pins: INTA and INTB.
Bydefault, INTA is associated with GPAn pins (PORTA)
and INTB is associated with GPBn pins (PORTB).
Each port has an independent signal which is cleared if
its associated GPIO or INTCAP register is read.

IMPORTANT:
If INTn pin pins are configured to mirror each other
the interrupt will only be cleared if both associated registers are read.

> Look at section 3.6.1 of the datasheet for details

## Usage

> [!NOTE]
> The Driver leaves hardware-specific tasks to the end user.

### Prelude

```C
#include <stdint.h>

uint8_t mcp23017_device_address = 0x00;
uint8_t mcp23017_i2c_timeout = 10;
```

### Read

```C
uint8_t register_value;

HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Read(
    hi2c3, 
    mcp23017_device_address, 
    REGISTER_GPIOA,
    MCP23017_I2C_SIZE, 
    &register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017_i2c_timeout
);

uint8_t bit_value = mcp23017_get_register_bit(register_value, 3);
```

### Write

```C
uint8_t register_value = 0b00000000; /// Local copy

mcp23017_set_register_bit(&register_value, 3, 1);

HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
    hi2c3, 
    mcp23017_device_address, 
    REGISTER_GPIOA,
    MCP23017_I2C_SIZE,
    &register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017_i2c_timeout
);
```

### Interrupt

```C
/// Default values
uint8_t gpinten_register_value = 0b00000000;
uint8_t intcon_register_value = 0b00000000;
uint8_t defval_register_value = 0b00000000;
uint8_t iocon_register_value = 0b00000000;

mcp23017_set_it_on_pin(
    &gpinten_register_value,
    &intcon_register_value,
    &defval_register_value,
    &iocon_register_value,
    1,
    MCP23017_INT_ENABLED,
    MCP23017_INT_MODE_COMPARE,
    1,
    MCP23017_INT_MIRROR_ENABLED
);

HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
    hi2c3, 
    mcp23017_device_address, 
    MCP23017_REGISTER_GPINTENA,
    MCP23017_I2C_SIZE,
    &gpinten_register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017_i2c_timeout
);
HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
    hi2c3, 
    mcp23017_device_address, 
    MCP23017_REGISTER_INTCONA,
    MCP23017_I2C_SIZE,
    &defval_register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017_i2c_timeout
);
HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
    hi2c3, 
    mcp23017_device_address, 
    MCP23017_REGISTER_DEFVALA,
    MCP23017_I2C_SIZE,
    &iocon_register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017_i2c_timeout
);
HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
    hi2c3, 
    mcp23017_device_address, 
    MCP23017_REGISTER_IOCON,
    MCP23017_I2C_SIZE,
    &register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017_i2c_timeout
);
```