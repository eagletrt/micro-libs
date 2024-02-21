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

INTCAP -> get_interrupt_value_on_pin()
GPIO   -> read_value_on_pin()

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

/**
 * @struct MCP23017_t
 * @brief rapresents the MCP23017 device
 */
typedef struct {
    I2C_HandleTypeDef *hi2c; /// I2C handle used for communication.
    uint8_t device_address; /// I2C address of the MCP23017 device.
    uint8_t i2c_timeout; /// Timeout value for I2C communication in milliseconds
} MCP23017_t;

MCP23017_t mcp23017 = {
    .hi2c = /// handle
    .device_address = 0x00,
    .i2c_timeout = 1
}
```

### Read

```C
uint8_t register_value;

HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Read(
    mcp23017.hi2c, 
    mcp23017.device_address, 
    REGISTER_GPIOA,
    MCP23017_I2C_SIZE, 
    &register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017.i2c_timeout
);

uint8_t bit_value = get_register_bit(register_value, 3);
```

### Write

```C
uint8_t register_value; //previously read value

set_register_bit(&register_value, 3, 1);

HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
    mcp23017.hi2c, 
    mcp23017.device_address, 
    REGISTER_GPIOA,
    MCP23017_I2C_SIZE,
    &register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017.i2c_timeout
);
```

### Interrupt

```C
uint8_t gpinten_register_value;
uint8_t intcon_register_value;
uint8_t defval_register_value;
uint8_t iocon_register_value;

set_interrupt_on_pin(
    &gpinten_register_value,
    &intcon_register_value,
    &defval_register_value,
    &iocon_register_value,
    1,
    interrupt_setting_enabled,
    interrupt_mode_compare,
    1,
    interrupt_mirror_enabled
);

HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
    mcp23017.hi2c, 
    mcp23017.device_address, 
    REGISTER_GPIOA,
    MCP23017_I2C_SIZE,
    &gpinten_register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017.i2c_timeout
);
HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
    mcp23017.hi2c, 
    mcp23017.device_address, 
    REGISTER_GPIOA,
    MCP23017_I2C_SIZE,
    &defval_register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017.i2c_timeout
);
HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
    mcp23017.hi2c, 
    mcp23017.device_address, 
    REGISTER_GPIOA,
    MCP23017_I2C_SIZE,
    &iocon_register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017.i2c_timeout
);
HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
    mcp23017.hi2c, 
    mcp23017.device_address, 
    REGISTER_GPIOA,
    MCP23017_I2C_SIZE,
    &register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017.i2c_timeout
);
```