# MCP23017 I/O Expander - Driver

The MCP23017 is an IO expander. Unlike a multiplexer, it aims to emulate the functionalities of additional physical pins, allowing for features like interrupts.

**Features**:

- 16 `GPIO` pins divided in banks `A` and `B`.
- 2 pins `INTA` and `INTB` for the interrupts of each bank.

Through access to its registers by the I2C protocol, you can read from and write to the pins, as well as configure the microchip to your requirements.

## Index

- [Documentation](#documentation)
- [Usage](#usage)
- [Common Registers](#common-registers)

## Documentation

- [Datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf)

This driver provides functions to:

- [x] Read register bits
- [x] Write register bits
- [x] Configure interrupt on a single pin
- [x] Configure interrupt on multiple pins

### Device Address

`[S|0|1|0|0|A2|A1|A0|R/W|ACK]`

Start bit | Slave address fixed part | Slave address | R/W bit | ACK bit
---|---|---|---|---|
 `S` | `0 1 0 0` | `A2 A1 A0` | `R/W` | `ACK`

The MCP23017 has 3 pin that are used to determine the device address: `A2`, `A1`, `A0`.
They need to be externally biased.
Look at your schematic to know it they are set to high(`1`) or low(`0`).
When calculating the address you have to consider the `R/W` bit set to `0`.

All considered the general address is `0100XXX0`.
You just need to replace the `X` with `A2`, `A1` and `A0`.

### Registers

The MCP23017 uses 8bit registers. They are replicated for each port.
So there is the GPIO register for port A called GPIOA and the respective register GPIOB for port B.
Each bit of the register reflects the status or configuration of a pin.

| bit 7 | 6 | 5 | 4 | 3 | 2 | 1 | bit 0 |
|---|---|---|---|---|---|---|---|
| pin 7 | 6 | 5 | 4 | 3 | 2 | 1 | pin 0 |

> [!IMPORTANT]
> This drivers makes some assumptions on the device configuration. Please read carefully.

#### IOCON

##### BANK setting

This drivers assumes that the device is used in the default bank configuration which si BANK = 0

> [!WARNING]
> Changing che BANK setting brakes compatibility whit this driver

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

> [!IMPORTANT]
> If INTn pin pins are configured to mirror each other
> the interrupt will only be cleared if both associated registers are read.

> [!NOTE]
> Look at section 3.6.1 of the datasheet for details

#### Interrupt Mirror Setting

There is one interrupt pin for each port: INTA and INTB.
The mirror bit of IOCON register controls how the pins INTA and INTB behave relative to each other.

> [!NOTE]
> Look at section 3.5.6 of the datasheet for details

## Usage

> [!IMPORTANT]
> The driver provides functions to read and edit local register copies according to your need.
> It is up to the user to manage the communication via I²C.

> [!TIP]
> The header file of this library provides a lot of useful defines to make thins easier
>
> - Registers addresses
> - I2C functions parameters
> - MCP23017 settings

### Checklist

- [x] Find the device address.
- [x] Choose a timeout value for the I²C transmissions.
- [x] Understand what register you need to read from or write to.
  - the header file contains a define for each register address
- [x] Read a register
  - Using the I²C library from STM, read the register from the peripheral and store the value in a local variable.
  - If you need to know the value of a register for a single pin you can use the dedicated function `mcp23017_get_register_bit()` on the local copy
- [x] Write a register
  - Modify the local copy of the register value to your needs.
  - If you need to set the value of a register for a single pin you can use the dedicated function `mcp23017_set_register_bit()` on the local copy
  - Using the I²C library from STM, write the register value to the peripheral.

> [!IMPORTANT]
> When you write a register be sure to have an updated local copy of the register value. Otherwise you may overwrite past writes.

> [!TIP]
> Look at TABLE 3-5 in the documentation to find the default values for all registers

### Prelude

```C
#include <stdint.h>

uint8_t mcp23017_device_address = 0x40;
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

#### Set Interrupt on a single pin

```C
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
> You can avoid to write registers that you didn't change and are not going to change from their default values

```C
// Default values
uint8_t gpinten_register_value = 0b00000000;
uint8_t intcon_register_value = 0b00000000;
uint8_t defval_register_value = 0b00000000;

mcp23017_set_it_on_pin(
  &gpinten_register_value,
  &intcon_register_value,
  &defval_register_value,
  1,
  MCP23017_INT_ENABLED,
  MCP23017_INT_MODE_ON_CHANGE,
  0
);

// In this case only GPINTEN has been modified and so it's the only register that I have to write to the MCP.

HAL_StatusTypeDef HAL_Status = HAL_I2C_Mem_Write(
  hi2c3, 
  mcp23017_device_address, 
  MCP23017_REGISTER_GPINTENA,
  MCP23017_I2C_SIZE,
  &gpinten_register_value, 
  MCP23017_I2C_SIZE, 
  mcp23017_i2c_timeout
);
```

#### Set Interrupt on multiple pins

If you use `MCP23017_INT_MODE_COMPARE` you need to specify at once the value of the `DEFVAL` register. This is to explicit the compare value for all pins.

```C
uint8_t compare_value = 0b11110000;

void mcp23017_set_it_on_all_pins(
  &gpinten_register_value,
  &intcon_register_value,
  &defval_register_value,
  MCP23017_INT_ENABLED,
  MCP23017_INT_MODE_COMPARE,
  compare_value
);

// ... Write to MCP registers by I2C ...
```

## Common Registers

### IODIR

Controls the direction of the data I/O.

### GPIO

The GPIO register reflects the value on the port.

### GPINTEN

> [!TIP]
> This register values can be configured with the dedicated `mcp23017_set_it_on_pin` function.

The GPINTEN register controls the
interrupt-on-change feature for each pin.

### INTCON

> [!TIP]
> This register values can be configured with the dedicated `mcp23017_set_it_on_pin` function.

The INTCON register controls how the associated pin
value is compared for the interrupt-on-change feature.

### DEFVAL

> [!TIP]
> This register values can be configured with the dedicated `mcp23017_set_it_on_pin` function.

Default value register.

### IOCON

Configuration Register

### Interrupt Mirror Setting

- `MCP23017_INT_MIRROR_DISABLED`: Interrupts pin are separated. An interrupt on one port activates only the relative port pin.

- `MCP23017_INT_MIRROR_ENABLED`: The interrupt pins are mirrored. An interrupt on one port activate the interrupt pins of both ports.

```C
uint8_t iocon_register_value = 0b00000000;
    
mcp23017_set_register_bit(&iocon_register_value, MCP23017_MIRROR_BIT MCP23017_INT_MIRROR_ENABLED);

// ... Write to MCP registers by I2C ...
```
