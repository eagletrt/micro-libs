# Command format

Each command can be subdivided in two categories, based on addressing, and other four
based on the command type.

When the data is sent/received to/from the ltc the following bytes are used in order:

- 16 bits of command
- 16 bits of *PEC* calculated from the command
- 48 bits of data (can be either sent or received)
- 16 bits of *PEC* calculated from the data
- Other pairs of data + *PEC* for some commands

|      16 |  16 |   48 |  16 |     64 $n$ |
| ------- | --- | ---- | --- | ---------- |
| Command | PEC | Data | PEC | Data + PEC |

!!! info
    The poll commands use an arbitrary amount of bits for the data part
    and does not require the *PEC*

|      16 |  16 |         $n$ |
| ------- | --- | ----------- |
| Command | PEC | Poll result |

## Addressing

A command can be either in:

- [Broadcast](#broadcast) format
- [Address](#address) format

### Broadcast

The broadcast format consist in the first 5 bits of the command set to 0 and the
remaining 11 bits set to the command code.

The broadcast commands can be used with any configuration of the ltc6811 and the
data is sent to all the connected nodes "simultaneously".

| BYTE | BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 |  BIT 2 | BIT 1 | BIT 0 |
| ---- | ----- | ----- | ----- | ----- | ----- | ------ | ----- | ----- |
|    0 |     0 |     0 |     0 |     0 |     0 | CC[10] | CC[9] | CC[8] |
|    1 | CC[7] | CC[6] | CC[5] | CC[4] | CC[3] |  CC[2] | CC[1] | CC[0] |

Where `CC[i]` is the $i$-th bit of the actual command code.

### Address

The address format consist in the first bit being a 0, the next 4 bits being the
address of the node and the remaining 11 bits being the command code.

!!! warning
    This format does not work with the daisy-chain configuration because chained
    nodes are not addressable

The address format can be used to communicate with a specific node in the network
up to 16 possible nodes.

| BYTE | BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 |  BIT 2 | BIT 1 | BIT 0 |
| ---- | ----- | ----- | ----- | ----- | ----- | ------ | ----- | ----- |
|    0 |     0 |  A[3] |  A[2] |  A[1] |  A[0] | CC[10] | CC[9] | CC[8] |
|    1 | CC[7] | CC[6] | CC[5] | CC[4] | CC[3] |  CC[2] | CC[1] | CC[0] |

Where `A[i]` is the $i$-th bit of the node address and `CC[i]` is the $i$-th bit
of the actual command code.

## Command type

There are three possible operations that can be done on the chip, and they are:

1. [Poll](#poll) operations
2. [Write](#write) operations
3. [Read](#read) operations

### Poll

The **poll command** can be used to check the status of some of the ltc's operations,
for example the status of the ADC conversions.

Once the command is sent the received data can be used to control the status of
the operation, this library let you check a single byte that can be either `0xFF`
if the operation has completed and `0x00` otherwise.

!!! warning
    In SPI communication if the chip select is pulled high the received data becomes
    `0xFF` even if the operation has not completed

The following example checks if a previously started ADC operation has ended on a
STM32 microcontroller using the HAL library:
```c
/* ... */

// Encode poll command
uint8_t poll_data;
uint8_t data[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
uint32_t data_size = ltc6811_pladc_encode_broadcast(&chain, data);

// Send and receive data via SPI
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
HAL_SPI_Transmit(&hspi, data, data_size, timeout);
HAL_SPI_Receive(&hspi, &poll_data, 1, timeout);
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

// Check for poll status
if (ltc6811_pladc_check(poll_data))
    // ...

/* ... */
```

### Write

The **write command** can be used to send data to one or multiple ltcs.

To send the data, pass all the required parameters to the appropriate encode function
and send the data via the chosen peripheral.

!!! warning
    There is no way to check if the data was received correctly by the LTCs other than
    read it back from the chip itself

When data is sent to multiple chips the first packet (data + PEC) is received by
the last node, the second is received by the second to last and so on.

The following example writes data on the configuration register of the LTCs on a
STM32 microcontroller using the HAL library:
```c
/* ... */

// Encode write command
Ltc6811Cfgr config[LTC_COUNT] = { /* ... */ };
uint8_t data[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
uint32_t data_size = ltc6811_wrcfg_encode_broadcast(&chain, config, data);

// Send data via SPI
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
HAL_SPI_Transmit(&hspi, data, data_size, timeout);
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

/* ... */
```

### Read

The **read command** can be used to receive data from one or multiple ltcs.

To receive data from the LTCs first encode the read command and send it via the chosen peripheral,
then receive the data and pass them to the right decode function.

When data is received from multiple chips the first packet (data + PEC) is sent from
the first node, the second is sent by the second and so on.

The following example reads data from the configuration register of the LTCs on a
STM32 microcontroller using the HAL library:
```c
/* ... */

// Encode read command
uint8_t cmd[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
uint8_t data[LTC6811_DATA_BUFFER_SIZE(LTC_COUNT)];
uint32_t data_size = ltc6811_rdcfg_encode_broadcast(&chain, cmd);

// Send and receive data via SPI
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
HAL_SPI_Transmit(&hspi, cmd, data_size, timeout);
HAL_SPI_Receive(&hspi, data, LTC6811_DATA_BUFFER_SIZE(LTC_COUNT), timeout);
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

// Decode received data
Ltc6811fgr config[LTC_COUNT];
size_t decoded_bytes = ltc6811_rdcfg_decode_broadcast(&chain, data, config);

/* ... */
```
