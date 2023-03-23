/**
 * @file		ltc6811.h
 * @brief		This file contains the functions to communicate with the LTCs
 *
 * @date		Mar 15, 2023
 * @author		Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#ifndef LTC6811_H
#define LTC6811_H

#include <inttypes.h>

#include "monitor_config.h"

#define LTC6811_REG_COUNT 4         // Number of Cell Voltage Register Groups (CVR[A-D])
#define LTC6811_REG_CELL_COUNT 3    // Number of cells handled by a single register (C[X]V)
#define LTC6811_CELL_COUNT (LTC6811_REG_COUNT * LTC6811_REG_CELL_COUNT) // Total number of cells handled by all registers (C[1-12]V)

/**
 * @brief ADC conversion mode
 * Set ADCOPT(CFGR0[0]) to:
 *  0 -> selects modes 26Hz, 422Hz, 7kHz or 27kHz (default)
 *  1 -> selects modes 1kHz, 2kHz, 3kHz or 14kHz
 */
typedef enum {
    MD_422HZ_1KHZ = 0,
    MD_27KHZ_14KHZ,
    MD_7KHZ_3KHZ,
    MD_26HZ_2KHZ
} LTC6811_MD;

/** @brief Permit measurement during discharge */
typedef enum {
    DCP_DISABLED = 0,
    DCP_ENABLED
} LTC6811_DCP;

/** @brief Cell selection for ADC conversion */
typedef enum {
    CELL_CH_ALL = 0,
    CELL_CH_1,
    CELL_CH_2,
    CELL_CH_3,
    CELL_CH_4,
    CELL_CH_5,
    CELL_CH_6
} LTC6811_CH;

/** @brief Pull-Up/Pull-Down current for Open Wire conversions */
typedef enum {
    PUP_INACTIVE = 0,
    PUP_ACTIVE
} LTC6811_PUP;

/**
 * @brief Indices of the cells to discharge
 * @details DCCs 1 through 8 can be found in CFGR4[0:7]
 *          DCCs 9 through 12 can be found in CFGR5[0:3]
 */
typedef enum {
    // CFGR4
    DCC1 = 1,
    DCC2,
    DCC3,
    DCC4,
    DCC5,
    DCC6,
    DCC7,
    DCC8,
    // CFGR5
    DCC9 = 1,
    DCC10,
    DCC11,
    DCC12
} LTC6811_DCC;

/** @brief Read cell voltage register group commands */
typedef enum {
    RDCVA = 0b0100,
    RDCVB = 0b0110,
    RDCVC = 0b1000,
    RDCVD = 0b1010,
    RDCVE = 0b1001,
    RDCVF = 0b1011,
} LTC6811_RDCV;

/**
 * @brief Discharge timeout value
 * @details DCTO[0:3] can be found in CFGR5[4:7]
 */
typedef enum {
    DCTO_DISABLED = 0,
    DCTO_30S,
    DCTO_1M,
    DCTO_2M,
    DCTO_3M,
    DCTO_4M,
    DCTO_5M,
    DCTO_10M,
    DCTO_15M,
    DCTO_20M,
    DCTO_30M,
    DCTO_40M,
    DCTO_60M,
    DCTO_75M,
    DCTO_90M,
    DCTO_120M
} LTC6811_DCTO;

/** @brief Write configuration register commands */
typedef enum {
    WRCFGA = 0,
    WRCFGB      // Forward compatibility with ltc6813/6812
} LTC6811_WRCFG;
/** @brief Read configuration register commands */
typedef enum {
    RDCFGA = 0,
    RDCFGB      // Forward compatibility with ltc6813/6812
} LTC6811_RDCFG;


/** @brief Table used to calculate the pec for messaging */
static const uint16_t crcTable[] = {
    0x0000, 0xc599, 0xceab, 0x0b32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac, 0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa,
    0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1, 0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d,
    0x5b2e, 0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b, 0x77e6, 0xb27f, 0xb94d, 0x7cd4,
    0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd, 0x2544, 0x2be,  0xc727, 0xcc15, 0x98c,  0xda71, 0x1fe8, 0x14da, 0xd143,
    0xf3c5, 0x365c, 0x3d6e, 0xf8f7, 0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2,  0xc25b, 0xc969, 0xcf0,  0xdf0d, 0x1a94, 0x11a6, 0xd43f,
    0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf, 0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8,
    0xa8eb, 0x6d72, 0x6640, 0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba, 0x4a88, 0x8f11,
    0x57c,  0xc0e5, 0xcbd7, 0xe4e,  0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b, 0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286,
    0xa213, 0x678a, 0x6cb8, 0xa921, 0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070, 0x85e9,
    0xf84,  0xca1d, 0xc12f, 0x4b6,  0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528, 0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e,
    0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59, 0x2ac0, 0xd3a,  0xc8a3, 0xc391, 0x608,  0xd5f5, 0x106c, 0x1b5e, 0xdec7,
    0x54aa, 0x9133, 0x9a01, 0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9, 0x7350,
    0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a, 0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c,
    0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25, 0x2fbc, 0x846,  0xcddf, 0xc6ed, 0x374,  0xd089, 0x1510, 0x1e22, 0xdbbb,
    0xaf8,  0xcf61, 0xc453, 0x1ca,  0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b, 0x2d02,
    0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3, 0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
};

/** @brief Discharge timeout in milliseconds */
// static const uint32_t DCTO_TO_MILLIS[] = {
//     0,        // disabled
//     30000,    // 30 sec
//     60000,    // 1 min
//     120000,   // 2 min
//     180000,   // 3 min
//     240000,   // 4 min
//     300000,   // 5 min
//     600000,   // 10 min
//     900000,   // 15 min
//     1200000,  // 20 min
//     1800000,  // 30 min
//     2400000,  // 40 min
//     3600000,  // 60 min
//     4500000,  // 75 min
//     5400000,  // 90 min
//     7200000   // 120 min
// };

/**
 * @brief Enable Chip Select
 * @param spi The spi configuration structure
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 */
void ltc6811_enable_cs(SPI_HandleTypeDef * spi, GPIO_TypeDef * gpio, uint16_t pin);
/**
 * @brief Disable Chip Select
 * @param spi The spi configuration structure
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 */
void ltc6811_disable_cs(SPI_HandleTypeDef * spi, GPIO_TypeDef * gpio, uint16_t pin);

/**
 * @brief Wakes up all the devices connected to the isoSPI bus
 * @param spi The spi configuration structure
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 */
void ltc6811_wakeup_idle(SPI_HandleTypeDef * spi, GPIO_TypeDef * gpio, uint16_t pin);

/**
 * @brief Clear cell voltage register group
 * @details	CLRCELL command syntax:
 *
 * 					31    CMD0    23     CMD1    15  PEC  0
 * 					|- - - - - - -|- - - - - - - -|- ... -|
 * 					0 0 0 0 0 1 1 1 0 0 0 1 0 0 0 1
 * 				    - - - - -
 * 					 Address
 * 
 * @param spi The spi configuration structure
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 */
void ltc6811_clrcell(SPI_HandleTypeDef * spi, GPIO_TypeDef * gpio, uint16_t pin);
/**
 * @brief Start cell voltage ADC conversion and poll status
 * @details	ADCV command syntax:
 *
 * 					31    CMD0    23     CMD1    15  PEC  0
 * 					|- - - - - - -|- - - - - - - -|- ... -|
 * 					0 0 0 0 0 0 1 X X 1 1 X 0 X X X
 * 				    - - - - -     - -     -   - - -
 * 					 Address      MD     DCP   CH
 * 
 * @param spi The spi configuration structure
 * @param MD The ADC conversion mode
 * @param DCP Discharge permitted option
 * @param CH Cell channel selection
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 */
void ltc6811_adcv(SPI_HandleTypeDef * spi,
    LTC6811_MD MD,
    LTC6811_DCP DCP,
    LTC6811_CH CH,
    GPIO_TypeDef * gpio,
    uint16_t pin);
/**
 * @brief Start Open Wire ADC conversion and poll status
 * @details	ADOW command syntax:
 *
 * 					31    CMD0    23     CMD1    15  PEC  0
 * 					|- - - - - - -|- - - - - - - -|- ... -|
 * 					0 0 0 0 0 0 1 X X X 1 X 1 X X X
 * 				    - - - - -     - - -   -   - - -
 * 					 Address      MD PUP DCP   CH
 *
 * @param spi The spi configuration structure
 * @param PUP Pull-Up/Pull-Down current for Open Wire conversion
 * @param MD The ADC conversion mode
 * @param DCP Discharge permitted option
 * @param CH Cell channel selection,
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 */
void ltc6811_adow(SPI_HandleTypeDef * spi,
    LTC6811_PUP PUP,
    LTC6811_MD MD,
    LTC6811_DCP DCP,
    LTC6811_CH CH,
    GPIO_TypeDef * gpio,
    uint16_t pin);
/**
 * @brief Poll ADC conversion status
 * @details PLADC command syntax:
 * 
 * 					31    CMD0    23     CMD1    15  PEC  0
 * 					|- - - - - - -|- - - - - - - -|- ... -|
 * 					0 0 0 0 0 1 1 1 0 0 0 1 0 1 0 0
 * 				    - - - - -
 * 					 Address
 *
 * @param spi The spi configuration structure
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 * @return HAL_StatusTypeDef The ADC conversion status
 */
HAL_StatusTypeDef ltc6811_pladc(SPI_HandleTypeDef * spi, GPIO_TypeDef * gpio, uint16_t pin);
/**
 * @brief Write data to a configuration register group
 * @details WRCFG command syntax:
 * 
 * 					31    CMD0    23     CMD1    15  PEC  0
 * 					|- - - - - - -|- - - - - - - -|- ... -|
 * A group: 		0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 * B group: 		0 0 0 0 0 0 0 0 0 0 1 0 0 1 0 0
 * 				    - - - - -
 * 					 Address
 * 
 * @param spi The spi configuration structure
 * @param reg The configuration register group
 * @param cfgr Configuration data
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 */
void ltc6811_wrcfg(SPI_HandleTypeDef * spi,
    LTC6811_WRCFG reg,
    uint8_t cfgr[8],
    GPIO_TypeDef * gpio,
    uint16_t pin);

// TODO: Implement RDCFG
uint8_t ltc6811_rdcfg(SPI_HandleTypedef * spi,
    LTC6811_RDCFG reg,
    GPIO_TypeDef * gpio,
    uint16_t pin);

/**
 * @brief Packet Error Code(PEC) calculation
 * @param data Message to be sent
 * @param len Number of bytes of the data
 */
uint16_t ltc6811_pec15(uint8_t data[], uint8_t len);

/**
 * @brief Polls all register and updates the volts array
 * @details It executes multiple rdcv commands and saves the value in the volts array
 *
 * 					31    CMD0    23     CMD1    15  PEC  0
 * 					|- - - - - - -|- - - - - - - -|- ... -|
 * 					0 0 0 0 0 0 0 0 0 0 0 0 X X X X  PEC
 * 					 Address                  
 * 					  (BRD)
 *
 * @param spi The SPI configuration structure
 * @param volts An array of 2 bytes where the result is stored
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 */
HAL_StatusTypeDef ltc6811_read_voltages(SPI_HandleTypeDef * spi,
    voltage_t volts[LTC6811_CELL_COUNT],
    GPIO_TypeDef * gpio,
    uint16_t pin);

/**
 * @brief Set the DCC bits in the Configuration Register array
 * 
 * @param cells Cells to discharge
 * @param cfgr The configuration register of the LTC6811
 */
void ltc6811_build_dcc(uint32_t cells, uint8_t cfgr[8]);
/**
 * @brief Write configuration for cell balancing to the LTC6811
 * 
 * @param spi The SPI configuration structure
 * @param cells Cells to balance
 * @param dcto Discharge timeout
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 */
void ltc6811_set_balancing(SPI_HandleTypeDef * spi,
    uint32_t cells,
    LTC6811_DCTO dcto,
    GPIO_TypeDef * gpio,
    uint16_t pin);

#endif /* LTC6811_H */