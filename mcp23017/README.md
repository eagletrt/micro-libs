# MCP23017 I/O Expander - Driver

## Documentation

- [Datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf)

This driver provides functions to:
- [x] Read registers
- [x] Write registers
- [x] Configure interrupts on pins

Additional documentation can be found in the file `mcp23017.h` as comments.

### BANK Setting

This drivers assumes that the device is used in the default bank configuration which si BANK = 0. Changing che BANK setting brakes compatibility whit this driver.

### Interrupts

#### Get interrupt info

The INTCAP register captures the GPIO port value at
the time the interrupt occurred. The register is
read-only and is updated only when an interrupt
occurs. The register remains unchanged until the
interrupt is cleared via a read of INTCAP or GPIO.

> [!NOTE]
> Look at section 3.5.9 of the datasheet for details

#### Interrupt Logic

There are two interrupt pins: INTA and INTB.
By default, INTA is associated with GPAn pins (PORTA)
and INTB is associated with GPBn pins (PORTB).
Each port has an independent signal which is cleared if
its associated GPIO or INTCAP register is read.


> [!WARNING]
> IMPORTANT:
> If INTn pin pins are configured to mirror each other
> the interrupt will only be cleared if both associated registers are read.

> [!NOTE]
> Look at section 3.6.1 of the datasheet for details

#### Interrupt Mirror Setting

There is one interrupt pin for each port: INTA and INTB.
The mirror bit controls how the pins INTA and INTB behave relative to each other.

> [!NOTE]
> Look at section 3.5.6 of the datasheet for details

## Usage

> [!IMPORTANT]
> The driver provides functions to read and edit local register copies according to your need.
> You are responsible to handle communication using the I²C protocol.

> [!TIP]
> The header file also provides a lot of useful defines:
> - Registers addresses
> - I2C functions parameters
> - MCP23017 settings

Checklist:

- [x] Find the device address.
  - the mcp23017 has 3 pin that are used to determine the device address. They need to be externally biased. Look at your schematic. The range of the address is `0x00`-`0x07` (2³)
- [x] Choose a timeout value for the I²C transmissions.
- [x] Understand what register you need to read from or write to.
  - the header file contains useful defines for each register.
- [x] Read a register
  - Using the I²C library from STM read from the register you want and store the value in a local variable.
  - If you need to know the value for a single pin you can use the dedicated function: `mcp23017_get_register_bit()`
- [x] Write a register
  - Modify the local copy of the register value using to your needs.
  - If you need to set the value for a single pin you can use the dedicated function: `mcp23017_set_register_bit()`
  - Using the I²C library from STM write the register value to the register you want.

> [!IMPORTANT]
> When you write a register be sure to have an updated local copy of the register value. Otherwise you may overwrite past writes.

> [!TIP]
> Look at TABLE 3-5 in the documentation to find the default values for all registers

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
uint8_t register_value = 0b00000000; // Local copy

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

### Interrupts

```C
/// Default values
uint8_t gpinten_register_value = 0b00000000;
uint8_t intcon_register_value = 0b00000000;
uint8_t defval_register_value = 0b00000000;

mcp23017_set_it_on_pin(
    &gpinten_register_value,
    &intcon_register_value,
    &defval_register_value,
    1,
    MCP23017_INT_ENABLED,
    MCP23017_INT_MODE_COMPARE,
    1
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
    &intcon_register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017_i2c_timeout
);
HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
    hi2c3, 
    mcp23017_device_address, 
    MCP23017_REGISTER_DEFVALA,
    MCP23017_I2C_SIZE,
    &defval_register_value, 
    MCP23017_I2C_SIZE, 
    mcp23017_i2c_timeout
);
```

> [!TIP]
> You can avoid to write registers that you don't change from their default values

### Interrupt Mirror Setting

- `MCP23017_INT_MIRROR_DISABLED`: Interrupts pin are separated. An interrupt on one port activates only the relative port pin.

- `MCP23017_INT_MIRROR_ENABLED`: The interrupt pins are mirrored. An interrupt on one port activate the interrupt pins of both ports.

```C
uint8_t iocon_register_value = 0b00000000;
    
mcp23017_set_register_bit(&iocon_register_value, MCP23017_MIRROR_BIT MCP23017_INT_MIRROR_ENABLED);
```