# M9525-WMN6P SPI EEPROM driver

Adapted from [this](https://github.com/firebull/STM32-EEPROM-SPI) library from Nikita Bulaev.

## Usage
Make sure that the `HOLD` pin of the EEPROM is set to `HIGH`.
`m95256_t` is the handle of an EEPROM instance. After you have declared an instance, initialize it with `m95256_init()` and provide the SPI device and the Chip Select pin.

You can use `m95256_WriteBuffer()`, `m95256_WritePage()` to write, and `m95256_ReadBuffer()` to read from the memory. More information can be found in the comments of those functions.
