# Documentation

[Datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf)


### API

The number indicates the section of the Datasheet.

- [x] 3.5.1 I/O DIRECTION REGISTER (IODIR)
- [x] 3.5.2 INPUT POLARITY REGISTER (IPOL)
- [x] 3.5.3 INTERRUPT-ON-CHANGE CONTROL REGISTER (GPINTEN)
- [x] 3.5.4 DEFAULT COMPARE REGISTER FOR INTERRUPT-ON-CHANGE (DEFVAL)
- [x] 3.5.5 INTERRUPT CONTROL REGISTER (INTCON)
- [x] 3.5.6 CONFIGURATION REGISTER (IOCON)
- [x] 3.5.7 PULL-UP RESISTOR CONFIGURATION REGISTER (GPPU)
- [x] 3.5.8 INTERRUPT FLAG REGISTER (INTF)
- [x] 3.5.9 INTERRUPT CAPTURED REGISTER (INTCAP)
- [x] 3.5.10 PORT REGISTER (GPIO)
- [x] 3.5.11 OUTPUT LATCH REGISTER (OLAT)

### Notes

#### Usage

Documentation can be found in file `mcp23017.h` as comments.

##### Implementation

An idea at the beginning was to use [bit-fields](https://en.cppreference.com/w/c/language/bit_field). Unfortunately it's not a viable solution because the [layout in memory is compiler dependant](https://stackoverflow.com/questions/15136426/memory-layout-of-struct-having-bitfields).
There are ways around this but in the end I decided to scrap the idea entirely.
I leave this information here for future developers.

```C
typedef struct {
    unsigned int bit_0 : 1,
    bit_1 : 1,
    bit_2 : 1,
    bit_3 : 1,
    bit_4 : 1,
    bit_5 : 1,
    bit_6 : 1,
    bit_7 : 1;
} BYTE_t;

typedef struct {
    BYTE_t value;
    uint8_t address;
} REGISTER_t;

struct REGISTERS {
    REGISTER_t IODIRA,
    ...
};
```