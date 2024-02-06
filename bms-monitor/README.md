# BMS-MONITOR

This library contains all the functions that can be used to communicate with the LTC6811,
a 12-cell battery stack monitor chip, which is the core component of the BMS monitor board. \
For more information refer to its [datasheet](/docs/bms-monitor/assets/ltc6811-1-6811-2.pdf)

The LTC6811 support three command types:
1. Write commands
2. Read commands
3. Poll commands

Each command can be either sent as:
1. Broadcast commands
2. Address commands (Does not work for daisy chain configuration)

For each command there are 1 to 2 possible function:
1. **Encode** function
2. **Decode** function


## Usage

> [!IMPORTANT]
> If SPI is used to send and receive commands the Chip Select(CS) line should always be
> set to low for the entire duration of the trasmission (reception included) and at the
> end it have to be reset to high

Based on the configuration of the LTC6811 a different handler between the `Ltc6811Chain` and `Ltc6811Array`
can be used, in any case remember to initialize it using the appropriate init function and parameters.

### Encode function

To send a command to the LTC it is first needed to call its related encode function
with the appropriate parameters and, eventually, the payload data to get the bytes to send to the chip. \
Then send the encoded bytes via the prefered peripheral (probably SPI).

For example, using an STM32 microcontroller:
```c
Ltc6811Cfgr config[LTC_COUNT];

/* Set configuration data... */

// Encode command data
uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
size_t byte_count = ltc6811_wrcfg_encode_broadcast(&chain, config, out);

// Send bytes
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PIN_RESET);
HAL_SPI_Transmit(&hspi, out, byte_count, spi_timeout); // Remember to use 'byte_count' as the data length
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PIN_SET);
```

### Decode function

If a read command is sent the payload data bytes can be received via the prefered peripheral (probably SPI) \
and can be decoded using the decode version of the command function used to read the data.


For example, using an STM32 microcontroller:
```c
// Encode command data
uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
size_t byte_count = ltc6811_rdcv_encode_broadcast(&chain, LTC6811_CVAR, out);

uint8_t read_bytes[LTC6811_DATA_BUFFER_SIZE(LTC_COUNT)];

// Send and receive data bytes
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PIN_RESET);
HAL_SPI_Transmit(&hspi, out, byte_count, spi_timeout); // Remember to use 'byte_count' as the data length
HAL_SPI_Receive(&hspi, read_bytes, LTC6811_DATA_BUFFER_SIZE(LTC_COUNT), spi_timeout);
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PIN_SET);

// Decode data
uint16_t voltages[LTC6811_REG_CELL_COUNT * LTC_COUNT] = { 0 };
ltc6811_rdcv_decode_broadcast(&chain, read_bytes, voltages);
```

