/**
 * @file ltc6811.h
 * @brief This file contains the functions to communicate with the LTC6811-1 chip
 *
 * @attention For SPI communication with the LTC, mode 3 is required (in CubeMX is CPOL="Low" and CPHA="2 edge")
 * and the MISO pin has to be set in Pull-Up mode (if it's not set on the hardware side)
 * Maximum SPI communication speed with the LTC is 1MHz
 * 
 * @details For additional information refer to the datasheet:
 * @link https://www.analog.com/media/en/technical-documentation/data-sheets/ltc6811-1-6811-2.pdf
 *
 * @date 16 Dec 2023
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 */

#ifndef LTC6811_H
#define LTC6811_H

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>

// Number of bytes in a register
#define LTC6811_REG_BYTE_COUNT 6U
// Number of bytes of a command (without PEC)
#define LTC6811_CMD_BYTE_COUNT 2U
// Number of bytes of the PEC
#define LTC6811_PEC_BYTE_COUNT 2U
// Number of bytes of a poll response
#define LTC6811_POLL_BYTE_COUNT 1U

// Time required for the isoSPI to go to the IDLE state (ms)
#define LTC6811_T_IDLE 5U
// Time required for the LTC6811 to go the SLEEP state (ms)
#define LTC6811_T_SLEEP 2000U

// Require number of clock cycles for an STCOMM command completion
#define LTC6811_STCOMM_CYCLES 72


/*** CELL VOLTAGE REGISTER ***/

// Number of cells voltages contained in a single register (C[X]V)
#define LTC6811_REG_CELL_COUNT 3
// Total number of cells voltages contained in all registers (CV)
#define LTC6811_CELL_COUNT (LTC6811_CVXR_COUNT * LTC6811_REG_CELL_COUNT)

/*** AUXILIARY VOLTAGE REGISTER ***/

// Number of auxiliary voltages contained in a single register (G[X]V)
#define LTC6811_REG_AUX_COUNT 3
// Total number of auxiliary voltages contained in all registers (GV)
#define LTC6811_AUX_COUNT (LTC6811_AVXR_COUNT * LTC6811_REG_AUX_COUNT)

/*** S PIN CONTROL REGISTER ***/

// Total number of S pin control values (SCTL[X])
#define LTC6811_SCTL_COUNT 12

/*** PWM REGISTER ***/

// Total number of PWM values (PWM[X])
#define LTC6811_PWM_COUNT 12

/*** COMM REGISTER ***/

// Maximum number of bytes that can be written or read from/to the COMM register (D[X])
#define LTC6811_COMM_DATA_COUNT 3

/**
 * @brief Get the maximum buffer size of an encoded write command in bytes
 * 
 * @param COUNT The total number of LTCs to write to
 * @return size_t The maximum buffer size (Bytes)
 */
#define LTC6811_WRITE_BUFFER_SIZE(COUNT) ((LTC6811_CMD_BYTE_COUNT) + (LTC6811_PEC_BYTE_COUNT) + ((LTC6811_REG_BYTE_COUNT) + (LTC6811_PEC_BYTE_COUNT)) * (COUNT))
/**
 * @brief Get the maximum buffer size of an encoded read command in bytes
 * 
 * @param COUNT The total number of LTCs to read from
 * @return size_t The maximum buffer size (Bytes)
 */
#define LTC6811_READ_BUFFER_SIZE(COUNT) ((LTC6811_CMD_BYTE_COUNT) + (LTC6811_PEC_BYTE_COUNT))
/**
 * @brief Get the maximum buffer size of the received data from the LTC in bytes
 *
 * @param COUNT The total number of LTCs to read from
 * @return size_t The maximum buffer size (Bytes)
 */
#define LTC6811_DATA_BUFFER_SIZE(COUNT) (((LTC6811_REG_BYTE_COUNT) + (LTC6811_PEC_BYTE_COUNT)) * COUNT)
/**
 * @brief Get the maximum buffer size of an encoded poll command in bytes
 *
 * @param COUNT The total number of LTCs to poll from
 * @return The maximum buffer size (Bytes)
 */
#define LTC6811_POLL_BUFFER_SIZE(COUNT) ((LTC6811_CMD_BYTE_COUNT) + (LTC6811_PEC_BYTE_COUNT))
/**
 * @brief Get the maximum buffer size of an encoded 'STCOMM' command in bytes
 *
 * @param COUNT The total number of LTCs to poll from
 * @return The maximum buffer size (Bytes)
 */
#define LTC6811_STCOMM_BUFFER_SIZE(COUNT) ((LTC6811_CMD_BYTE_COUNT) + (LTC6811_PEC_BYTE_COUNT) + (LTC6811_STCOMM_CYCLES))

/**
 * @brief List of available commands for communication with the LTC6811
 * @details Each command maked with '*' is provided for forward-compatibility with LTC6813/6812
 */
typedef enum {
    WRCFGA   = 0b00000000001,  // Write configuration register group A
    WRCFGB   = 0b00000100100,  // Write configuration register group B *
    RDCFGA   = 0b00000000010,  // Read configuration register group A
    RDCFGB   = 0b00000100110,  // Read configuration register group B *
    RDCVA    = 0b00000000100,  // Read cell voltage register group A
    RDCVB    = 0b00000000110,  // Read cell voltage register group B
    RDCVC    = 0b00000001000,  // Read cell voltage register group C
    RDCVD    = 0b00000001010,  // Read cell voltage register group D
    RDCVE    = 0b00000001001,  // Read cell voltage register group E *
    RDCVF    = 0b00000001011,  // Read cell voltage register group F *
    RDAUXA   = 0b00000001100,  // Read auxiliary register group A
    RDAUXB   = 0b00000001110,  // Read auxiliary register group B
    RDAUXC   = 0b00000001101,  // Read auxiliary register group C *
    RDAUXD   = 0b00000001111,  // Read auxiliary register group D *
    RDSTATA  = 0b00000010000,  // Read status register group A
    RDSTATB  = 0b00000010010,  // Read status register group B
    WRSCTRL  = 0b00000010100,  // Write S control register group
    WRPWM    = 0b00000100000,  // Write PWM register group
    WRPSB    = 0b00000011100,  // Write PWM/S control register group B *
    RDSCTRL  = 0b00000010110,  // Read S control register group
    RDPWM    = 0b00000100010,  // Read PWM register group
    RDPSB    = 0b00000011110,  // Read PWM/S control register group B *
    STSCTRL  = 0b00000011001,  // Start S control pulsing and poll status
    CLRSCTRL = 0b00000011000,  // Clear S control register group
    ADCV     = 0b01001100000,  // Start cell voltage ADC conversion and poll status
    ADOW     = 0b01000101000,  // Start open wire ADC conversion and poll status
    CVST     = 0b01000000111,  // Start self test cell voltage conversion and poll status
    ADOL     = 0b01000000001,  // Start overlap measurement of cell 7 voltage
    ADAX     = 0b10001100000,  // Start GPIOs ADC conversion and poll status
    ADAXD    = 0b10000000000,  // Start GPIOs ADC with digital redundancy and poll status
    AXST     = 0b10000000111,  // Start self test GPIOs conversion and poll status
    ADSTAT   = 0b10001101000,  // Start status group ADC conversion and poll status
    ADSTATD  = 0b10000001000,  // Start status group ADC conversion with digital redundancy and poll status
    STATST   = 0b10000001111,  // Start self test status group conversion and poll status
    ADCVAX   = 0b10001101111,  // Start combined cell voltage and GPIO1, GPIO2 conversion and poll status
    ADCVSC   = 0b10001100111,  // Start combined cell voltage and SC conversion and poll status
    CLRCELL  = 0b11100010001,  // Clear cell voltage register groups
    CLRAUX   = 0b11100010010,  // Clear auxiliary register groups
    CLRSTAT  = 0b11100010011,  // Clear status register groups
    PLADC    = 0b11100010100,  // Poll ADC conversion status
    DIAGN    = 0b11100010101,  // Diagnose MUX and poll status
    WRCOMM   = 0b11100100001,  // Write COMM register group
    RDCOMM   = 0b11100100010,  // Read COMM register group
    STCOMM   = 0b11100100011   // Start I2C/SPI communication
} Ltc6811Command;

/** @brief Configuration register group */
typedef enum {
    LTC6811_CFGAR = 0,
    LTC6811_CFGXR_COUNT
} Ltc6811Cfgxr;
/** @brief Cell voltage register group */
typedef enum {
    LTC6811_CVAR = 0,
    LTC6811_CVBR,
    LTC6811_CVCR,
    LTC6811_CVDR,
    LTC6811_CVXR_COUNT
} Ltc6811Cvxr;
/** @brief Auxiliary voltage register group */
typedef enum {
    LTC6811_AVAR = 0,
    LTC6811_AVBR,
    LTC6811_AVXR_COUNT
} Ltc6811Avxr;
/** @brief Status register group */
typedef enum {
    LTC6811_STAR = 0,
    LTC6811_STBR,
    LTC6811_STXR_COUNT
} Ltc6811Stxr;
/** @brief Communication register group */
typedef enum {
    LTC6811_COMMA = 0,
    LTC6811_COMMX_COUNT
} Ltc6811Commx;
/** @brief PWM register group */
typedef enum {
    LTC6811_PWMAR = 0,
    LTC6811_PWMXR_COUNT
} Ltc6811Pwmxr;

/**
 * @brief ADC conversion mode
 * @details Set the ADCOPT in the LTC configuration to:
 *  0 -> selects modes 26Hz, 422Hz, 7kHz or 27kHz (default)
 *  1 -> selects modes 1kHz, 2kHz, 3kHz or 14kHz
 */
typedef enum {
    LTC6811_MD_422HZ_1KHZ = 0,
    LTC6811_MD_27KHZ_14KHZ,
    LTC6811_MD_7KHZ_3KHZ,
    LTC6811_MD_26HZ_2KHZ
} Ltc6811Md;
/** @brief Pull-Up/Pull-Down current for Open Wire conversions */
typedef enum {
    LTC6811_PUP_INACTIVE = 0,
    LTC6811_PUP_ACTIVE
} Ltc6811Pup;
/** @brief Self test mode selection */
typedef enum {
    LTC6811_ST_ONE = 1,
    LTC6811_ST_TWO
} Ltc6811St;
/** @brief Permit measurement during discharge */
typedef enum {
    LTC6811_DCP_DISABLED = 0,
    LTC6811_DCP_ENABLED
} Ltc6811Dcp;
/** @brief Cell selection for ADC conversion */
typedef enum {
    LTC6811_CH_ALL = 0,
    LTC6811_CH_1,
    LTC6811_CH_2,
    LTC6811_CH_3,
    LTC6811_CH_4,
    LTC6811_CH_5,
    LTC6811_CH_6,
    LTC6811_CH_7 = LTC6811_CH_1,
    LTC6811_CH_8 = LTC6811_CH_2,
    LTC6811_CH_9 = LTC6811_CH_3,
    LTC6811_CH_10 = LTC6811_CH_4,
    LTC6811_CH_11 = LTC6811_CH_5,
    LTC6811_CH_12 = LTC6811_CH_6
} Ltc6811Ch;
/** @brief GPIO selection for ADC conversion */
typedef enum {
    LTC6811_CHG_GPIO_ALL = 0,
    LTC6811_CHG_GPIO_1,
    LTC6811_CHG_GPIO_2,
    LTC6811_CHG_GPIO_3,
    LTC6811_CHG_GPIO_4,
    LTC6811_CHG_GPIO_5,
    LTC6811_CHG_SECOND_REF
} Ltc6811Chg;
/** @brief Status group selection */
typedef enum {
    LTC6811_CHST_ALL = 0,
    LTC6811_CHST_SC,
    LTC6811_CHST_ITMP,
    LTC6811_CHST_VA,
    LTC6811_CHST_VD
} Ltc6811Chst;
/** @brief Discharge timeout values in minutes */
typedef enum {
    LTC6811_DCTO_OFF = 0, // Discharge is disabled
    LTC6811_DCTO_30S,
    LTC6811_DCTO_1MIN,
    LTC6811_DCTO_2MIN,
    LTC6811_DCTO_3MIN,
    LTC6811_DCTO_4MIN,
    LTC6811_DCTO_5MIN,
    LTC6811_DCTO_10MIN,
    LTC6811_DCTO_15MIN,
    LTC6811_DCTO_20MIN,
    LTC6811_DCTO_30MIN,
    LTC6811_DCTO_40MIN,
    LTC6811_DCTO_60MIN,
    LTC6811_DCTO_75MIN,
    LTC6811_DCTO_90MIN,
    LTC6811_DCTO_120MIN
} Ltc6811Dcto;

/** @brief I2C inital communication control bits on write */
typedef enum {
    LTC6811_I2C_WICOM_START = 0b0110, // Start signal that will be followed by data
    LTC6811_I2C_WICOM_STOP = 0b0001, // Stop signal
    LTC6811_I2C_WICOM_BLANK = 0b0000, // Proceed to data trasmission
    LTC6811_I2C_WICOM_NOTRANSMIT = 0b0111 // Release SDA and SCL and ignore the rest of data
} Ltc6811I2cWicom;
/** @brief I2C final communication control bits on write */
typedef enum {
    LTC6811_I2C_WFCOM_ACK = 0b0000, // Generate an ACK signal on ninth clock cycle
    LTC6811_I2C_WFCOM_NACK = 0b1000, // Generate a NACK signal on ninth clock cycle
    LTC6811_I2C_WFCOM_STOP = 0b1001, // Generate a NACK signal followed by STOP signal
} Ltc6811I2cWfcom;
/** @brief SPI inital communication control bits on write */
typedef enum {
    LTC6811_SPI_WICOM_LOW = 0b1000, // Generate a CSBM low signal on SPI port
    LTC6811_SPI_WICOM_FALLING = 0b1010, // Drive CSBM high and then low
    LTC6811_SPI_WICOM_HIGH = 0b1001, // Generate a CSBM high signal on SPI port
    LTC6811_SPI_WICOM_NOTRANSMIT = 0b1111, // Release the SPI port and ignore the rest of the data
} Ltc6811SpiWicom;
/**
 * @brief SPI final communication control bits on write
 * @attention The low value can be either 0b0000 or 0b1000
 */
typedef enum {
    LTC6811_SPI_WFCOM_LOW = 0b0000, // Holds CSBM low at the end of byte transmission
    LTC6811_SPI_WFCOM_LOW_B = 0b1000, // Holds CSBM low at the end of byte transmission
    LTC6811_SPI_WFCOM_HIGH = 0b1001 // Transitions CSBM high at the end of byte transmission
} Ltc6811SpiWfcom;
/** @brief I2C inital communication control bits on read */
typedef enum {
    LTC6811_I2C_RICOM_START = 0b0110, // Master generated a start signal
    LTC6811_I2C_RICOM_STOP = 0b0001, // Master generated a stop signal
    LTC6811_I2C_RICOM_BLANK = 0b0000, // SDA was held low between bytes
    LTC6811_I2C_RICOM_NOTRANSMIT = 0b0111 // SDA was held high between bytes
} Ltc6811I2cRicom;
/** @brief I2C final communication control bits on read */
typedef enum {
    LTC6811_I2C_RFCOM_MACK = 0b0000, // Master generated an ACK signal
    LTC6811_I2C_RFCOM_SACK = 0b0111, // Slave generated an ACK signal
    LTC6811_I2C_RFCOM_SNACK = 0b1111, // Slave generated a NACK signal
    LTC6811_I2C_RFCOM_SACK_STOP = 0b0001, // Slave generated an ACK and master generated a STOP signal
    LTC6811_I2C_RFCOM_SNACK_STOP = 0b1001 // Slave generated a NACK and master generated a STOP signal
} Ltc6811I2cRfcom;
/** @brief SPI inital communication control bits on read */
typedef enum {
    LTC6811_SPI_RICOM = 0b0111
} Ltc6811SpiRicom;
/** @brief SPI final communication control bits on read */
typedef enum {
    LTC6811_SPI_RFCOM = 0b1111
} Ltc6811SpiRfcom;


/** @brief Configuration register group structure */
typedef struct __attribute__((__packed__)) {
    uint8_t ADCOPT : 1; // ADC mode option bit
    uint8_t DTEN : 1;   // Discharge timer enable (READ ONLY)
    uint8_t REFON : 1;  // References sPowered up
    uint8_t GPIO : 5;   // GPIOx pin control (GPIO1 starts from the least significant bit)
    uint16_t VUV : 12;  // Undervoltage comparison voltage
    uint16_t VOV : 12;  // Overvoltage comparison voltage
    uint16_t DCC : 12;  // Discharge cell x
    uint8_t DCTO : 4;   // Discharge time out value
} Ltc6811Cfgr;
/** @brief Status register group structure */
typedef struct __attribute__((__packed__)) {
    uint16_t SC;         // Sum of all cells measurement
    uint16_t ITMP;       // Internal die temperature
    uint16_t VA;         // Analog power supply voltage
    uint16_t VD;         // Digital power supply voltage
    uint16_t COV : 12;   // Cell over voltage flag
    uint16_t CUV : 12;   // Cell under voltage flag
    uint8_t REV : 4;     // Device revision code
    uint8_t RSVD : 2;    // Reserved bits (always 0s)
    uint8_t MUXFAIL : 1; // Multiplexer self test result
    uint8_t THSD : 1;    // Thermal shutdown status
} Ltc6811Str;
/**
 * @brief External communication register group structure
 *
 * @details The LTC6811 can be used as master for I2C and SPI communication through
 * its GPIOs
 * For I2C:
 *  - GPIO4 is used as the Serial DAta(SDA)
 *  - GPIO5 is used as the Serial CLock(SCL)
 * For SPI:
 *  - GPIO3 is used as the Chip Select(CSBM)
 *  - GPIO4 is used as the Serial Data Input-Output(SDIOM)
 *  - GPIO5 is used as the Serial Clock(SCKM)
 *
 * @attention For SPI only mode 3 (CHPA = 1, CPOL = 1) is supported
 */
typedef struct __attribute__((__packed__)) {
    uint8_t icom0 : 4; // Initial communication control bits of the first data byte
    uint8_t icom1 : 4; // Initial communication control bits of the second data byte
    uint8_t icom2 : 4; // Initial communication control bits of the third data byte
    uint8_t fcom0 : 4; // Final communication control bits of the first data byte
    uint8_t fcom1 : 4; // Final communication control bits of the second data byte
    uint8_t fcom2 : 4; // Final communication control bits of the third data byte
    uint8_t data[LTC6811_COMM_DATA_COUNT];   // Data transmited (received) to (from) I2C/SPI slave device
} Ltc6811Comm;
/** @brief LTC6811 handler structure */
typedef struct {
    size_t count;
} Ltc6811Chain;
typedef struct {
    size_t count;
} Ltc6811Array;


/**
 * @brief Initialize the LTC6811 chain structure
 *
 * @param chain The chain structure
 * @param ltc_count The total number of LTC6811 in the chain
 */
void ltc6811_chain_init(Ltc6811Chain * chain, size_t ltc_count);

/**
 * @brief Check if the ADC conversion has ended or not
 *
 * @attention Do not pull the Chip Select(CS) high before the conversion has ended
 * otherwise this command will not work
 *
 * @param byte The single byte read after the pladc command
 * @return bool True if the conversion has ended, false otherwise
 */
bool ltc6811_pladc_check(uint8_t byte);

/******************************************/
/*           BROADCAST COMMANDS           */
/******************************************/

/**
 * @brief Encode the configuration data of the LTC's to a broadcast write command
 * 
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_WRITE_BUFFER_SIZE macro to get the right size for the buffer
 * @attention The 'config' array size have to be equals to the number of LTC6811 in the chain
 *
 * @param chain The LTC6811 broadcast handler
 * @param config The configuration data
 * @param out The array where the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_wrcfg_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Cfgr * config,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast configuration read command
 * 
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_READ_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array where the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_rdcfg_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Decode the LTC broadcast configuration data
 *
 * @attention The 'out' array should be as large as the total number of LTC6811
 * in the chain
 *
 * @details The return value can be compared with the LTC6811_DATA_BUFFER_SIZE macro
 * to check if all bytes where decoded correctly
 *
 * @param chain The LTC6811 broadcast handler
 * @param data The array of bytes to decode
 * @param out The array where the configuration data is stored
 * @return size_t The number of decoded bytes (PEC included)
 */
size_t ltc6811_rdcfg_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    Ltc6811Cfgr * out
);
/**
 * @brief Encode the LTC broadcast cell voltage read command
 *
 * @attention The 'out' array should be large enough to contain all the encode bytes
 * Use the LTC6811_READ_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param reg The register to read from
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_rdcv_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Cvxr reg,
    uint8_t * out
);
/**
 * @brief Decode the LTC broadcast cell voltage data
 *
 * @attention The 'out' array should be large enough to store 'LTC6811_REG_CELL_COUNT'
 * voltages for each LTC6811 in the chain
 *
 * @details The return value can be compared with the LTC6811_DATA_BUFFER_SIZE macro
 * to check if all bytes where decoded correctly
 *
 * @param chain The LTC6811 broadcast handler
 * @param data The array of bytes to decode
 * @param out The array where the cell voltages are stored
 * @return size_t The number of decoded bytes (PEC included)
 */
size_t ltc6811_rdcv_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint16_t * out
);
/**
 * @brief Encode the LTC broadcast auxiliary voltage read command
 *
 * @attention The 'out' array should be large enough to contain all the encode bytes
 * Use the LTC6811_READ_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param reg The register to read from
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_rdaux_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Avxr reg,
    uint8_t * out
);
/**
 * @brief Decode the LTC broadcast auxiliary voltage data
 *
 * @attention The 'out' array should be large enough to store 'LTC6811_REG_AUX_COUNT'
 * voltages for each LTC6811 in the chain
 *
 * @details The return value can be compared with the LTC6811_DATA_BUFFER_SIZE macro
 * to check if all bytes where decoded correctly
 *
 * @param chain The LTC6811 broadcast handler
 * @param data The array of bytes to decode
 * @param out The array where the auxiliary voltages are stored
 * @return size_t The number of decoded bytes (PEC included)
 */
size_t ltc6811_rdaux_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint16_t * out
);
/**
 * @brief Encode the LTC broadcast status read command
 * 
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_READ_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param reg The register to read from
 * @param out The array where the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_rdstat_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Stxr reg,
    uint8_t * out
);
/**
 * @brief Decode the LTC broadcast status data
 *
 * @attention The 'out' array should be as large as the total number of LTC6811
 * in the chain
 *
 * @details The return value can be compared with the LTC6811_DATA_BUFFER_SIZE macro
 * to check if all bytes where decoded correctly
 *
 * @param chain The LTC6811 broadcast handler
 * @param reg The register where the data comes from
 * @param data The array of bytes to decode
 * @param out The array where the status data is stored
 * @return size_t The number of decoded bytes (PEC included)
 */
size_t ltc6811_rdstat_decode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Stxr reg,
    uint8_t * data,
    Ltc6811Str * out
);
/**
 * @brief Encode S pin control data of the LTC's into a broadcast write command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_WRITE_BUFFER_SIZE macro to get the right size for the buffer
 * @attention The 'data' array size have to be equal to the total number of
 * S pin control values of all the LTC's in the chain (LTC6811_SCTL_COUNT * chain.count)
 *
 * @param chain The LTC6811 broadcast handler
 * @param data The array of data to write
 * @param out The array where the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_wrsctrl_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast S pin control read command
 *
 * @attention The 'out' array should be large enough to contain all the encode bytes
 * Use the LTC6811_READ_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_rdsctrl_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Decode the LTC broadcast S pin control data
 *
 * @attention The 'out' array should be large enough to store 'LTC6811_SCTL_COUNT'
 * values for each LTC6811 in the chain
 *
 * @details The return value can be compared with the LTC6811_DATA_BUFFER_SIZE macro
 * to check if all bytes where decoded correctly
 *
 * @param chain The LTC6811 broadcast handler
 * @param data The array of bytes to decode
 * @param out The array where the S pin control data is stored
 * @return size_t The number of decoded bytes (PEC included)
 */
size_t ltc6811_rdsctrl_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint8_t * out
);
/**
 * @brief Encode PWM data of the LTC's into a broadcast write command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_WRITE_BUFFER_SIZE macro to get the right size for the buffer
 * @attention The 'data' array size have to be equal to the total number of
 * PWM values of all the LTC's in the chain (LTC6811_PWM_COUNT * chain.count)
 *
 * @param chain The LTC6811 broadcast handler
 * @param data The array of data to write
 * @param out The array where the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_wrpwm_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast pwm read command
 *
 * @attention The 'out' array should be large enough to contain all the encode bytes
 * Use the LTC6811_READ_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_rdpwm_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Decode the LTC broadcast S pin control data
 *
 * @attention The 'out' array should be large enough to store 'LTC6811_SCTL_COUNT'
 * values for each LTC6811 in the chain
 *
 * @details The return value can be compared with the LTC6811_DATA_BUFFER_SIZE macro
 * to check if all bytes where decoded correctly
 *
 * @param chain The LTC6811 broadcast handler
 * @param data The array of bytes to decode
 * @param out The array where the S pin control data is stored
 * @return size_t The number of decoded bytes (PEC included)
 */
size_t ltc6811_rdsctrl_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast start S pin control pulsing and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array where the encoded bytes are written
 * @return The number of encoded bytes
 */
size_t ltc6811_stsctrl_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Encode the LTC broacast clear S pin control register command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array where the encoded bytes are written
 * @return The number of encoded bytes
 */
size_t ltc6811_clrsctrl_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast pwm read command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_READ_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_rdpwm_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Decode the LTC broadcast pwm data
 *
 * @attention The 'out' array should be large enough to store 'LTC6811_PWM_COUNT'
 * values for each LTC6811 in the chain
 *
 * @details The return value can be compared with the LTC6811_DATA_BUFFER_SIZE macro
 * to check if all bytes where decoded correctly
 *
 * @param chain The LTC6811 broadcast handler
 * @param data The array of bytes to decode
 * @param out The array where the pwm data is stored
 * @return size_t The number of decoded bytes (PEC included)
 */
size_t ltc6811_rdpwm_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast cells ADC conversion start and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param dcp Allow (or not) measurement during discharge
 * @param cells The selected cells
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_adcv_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Dcp dcp,
    Ltc6811Ch cells,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast open wire ADC conversion start and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param pup Pull-up/Pull-down option
 * @param dcp Allow (or not) measurement during discharge
 * @param cells The selected cells
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_adow_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Pup pup,
    Ltc6811Dcp dcp,
    Ltc6811Ch cells,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast cell voltage conversion self test start and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param test_mode Self test mode option
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_cvst_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811St test_mode,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast cell 7 voltage overlap measurement start command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param dcp Allow (or not) measurement during discharge
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_adol_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Dcp dcp,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast GPIOs ADC conversion start and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param gpios The selected GPIOs
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_adax_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Chg gpios,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast GPIOs ADC conversion with digital redundancy start and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param gpios The selected GPIOs
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_adaxd_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Chg gpios,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast GPIOs conversion self test start and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param test_mode Self test mode option
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_axst_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811St test_mode,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast status group ADC conversion start and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param groups The selected status group
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_adstat_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Chst groups,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast status group ADC conversion with digital redundancy start and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param groups The selected status group
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_adstatd_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Chst groups,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast status group conversion self test start and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param test_mode Self test mode option
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_statst_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811St test_mode,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast cell voltage and GPIOs ADC conversion start and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param dcp Allow (or not) measurement during discharge
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_adcvax_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Dcp dcp,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast cell voltage and SC conversion start and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param mode The ADC conversion mode
 * @param dcp Allow (or not) measurement during discharge
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_adcvsc_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Dcp dcp,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast clear cell voltage register command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_clrcell_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast clear GPIOs voltage register command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_clraux_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast clear status register command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_clrstat_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast poll ADC conversion status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @details To check for the ADC conversion status send the encoded command and
 * then read a single byte and check the status using the 'ltc6811_pladc_check' function
 * until it has ended conversion
 *
 * @attention Do not pull the Chip Select(CS) high before the conversion has ended
 * otherwise this command will not work
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_pladc_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Encode the LTC broadcast diagnose MUX and poll status command
 *
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_POLL_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array were the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_diagn_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Encode the external communication data of the LTC's to a broadcast write command
 * 
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_WRITE_BUFFER_SIZE macro to get the right size for the buffer
 * @attention The 'comm' array size have to be equals to the number of LTC6811 in the chain
 *
 * @param chain The LTC6811 broadcast handler
 * @param comm The communication data
 * @param out The array where the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_wrcomm_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Comm * comm,
    uint8_t * out
);
/**
 * @brief Encode the external communication data of the LTC's broadcast read command
 * 
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_READ_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array where the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_rdcomm_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);
/**
 * @brief Decode the LTC broadcast external communication data
 *
 * @attention The 'out' array should be as large as the total number of LTC6811
 * in the chain
 *
 * @details The return value can be compared with the LTC6811_DATA_BUFFER_SIZE macro
 * to check if all bytes where decoded correctly
 *
 * @param chain The LTC6811 broadcast handler
 * @param data The array of bytes to decode
 * @param out The array where the communication data is stored
 * @return size_t The number of decoded bytes (PEC included)
 */
size_t ltc6811_rdcomm_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    Ltc6811Comm * out
);
/**
 * @brief Encode the external communication broadcast start command
 * 
 * @attention The 'out' array should be large enough to contain all the encoded bytes
 * Use the LTC6811_STCOMM_BUFFER_SIZE macro to get the right size for the buffer
 *
 * @param chain The LTC6811 broadcast handler
 * @param out The array where the encoded bytes are written
 * @return size_t The number of encoded bytes
 */
size_t ltc6811_stcomm_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out
);


/******************************************/
/*            ADDRESS COMMANDS            */
/******************************************/

#endif  // LTC6811_H

