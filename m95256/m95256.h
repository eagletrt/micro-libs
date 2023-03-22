/* Copyright 2017-2019 Nikita Bulaev
 *
 */

#ifndef _M95256_H
#define _M95256_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* M95040 SPI EEPROM defines */
#define EEPROM_WIP_FLAG 0x01 /*!< Write In Progress (WIP) flag */

#define EEPROM_PAGESIZE    32 /*!< Pagesize according to documentation */
#define EEPROM_BUFFER_SIZE 32 /*!< EEPROM Buffer size. Setup to your needs */

#define EEPROM_OPS_TIMEOUT 10

#define EEPROM_CS_HIGH(GPIO, PIN) HAL_GPIO_WritePin(GPIO, PIN, GPIO_PIN_SET)
#define EEPROM_CS_LOW(GPIO, PIN)  HAL_GPIO_WritePin(GPIO, PIN, GPIO_PIN_RESET)
#define EEPROM_HOLD_HIGH(GPIO, PIN)  HAL_GPIO_WritePin(GPIO, PIN, GPIO_PIN_SET)
#define EEPROM_HOLD_LOW(GPIO, PIN)  HAL_GPIO_WritePin(GPIO, PIN, GPIO_PIN_RESET)

/** @brief EEPROM comunication structure */
struct m95256 {
    SPI_HandleTypeDef * spi;
    GPIO_TypeDef * cs_gpio;
    uint16_t cs_pin;
};
typedef struct m95256 * m95256_t;

typedef enum {
    EEPROM_WREN  = 0b0110, // Write enable
    EEPROM_WRDI  = 0b0100, // Write disable
    EEPROM_RDSR  = 0b0101, // Read status register
    EEPROM_WRSR  = 0b0001, // Write status register
    EEPROM_READ  = 0b0011, // Read from the memory array
    EEPROM_WRITE = 0b0010  // Write to memory array
} EEPROM_INSTRUCTION;

/** @brief EEPROM Operations statuses */
typedef enum {
    EEPROM_STATUS_PENDING,
    EEPROM_STATUS_COMPLETE,
    EEPROM_STATUS_TIMEOUT,
    EEPROM_STATUS_ERROR
} EepromOperations;

/**
 * @brief Init EEPROM SPI
 *
 * @param eeprom EEPROM instance handle
 * @param spi Pointer to SPI struct handle
 * @param cs_gpio Pointer to the GPIO struct for the chip select pin
 * @param cs_pin Chip select pin number
 */
void m95256_init(m95256_t *eeprom,
    SPI_HandleTypeDef * spi,
    GPIO_TypeDef * cs_gpio,
    uint16_t cs_pin);
/**
 * @brief Deinit EEPROM SPI
 * 
 * @param eeprom EEPROM instance handle
 */
void m95256_deinit(m95256_t * eeprom);

/**
  * @brief  Writes more than one byte to the EEPROM with a single WRITE cycle
  *         (Page WRITE sequence).
  *
  * @note   The number of byte can't exceed the EEPROM page size.
  * 
  * @param	eeprom EEPROM instance handle
  * @param  pBuffer Pointer to the buffer  containing the data to be written
  *         to the EEPROM.
  * @param  WriteAddr EEPROM's internal address to write to.
  * @param  NumByteToWrite Number of bytes to write to the EEPROM, must be equal
  *         or less than "EEPROM_PAGESIZE" value.
  * @retval EepromOperations value: EEPROM_STATUS_COMPLETE or EEPROM_STATUS_ERROR
  */
EepromOperations m95256_WritePage(m95256_t eeprom,
    uint8_t * pBuffer,
    uint16_t WriteAddr,
    uint16_t NumByteToWrite);
/**
  * @brief  Writes block of data to the EEPROM. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  *
  * @param	eeprom EEPROM instance handle
  * @param  pBuffer Pointer to the buffer  containing the data to be written
  *         to the EEPROM.
  * @param  WriteAddr EEPROM's internal address to write to.
  * @param  NumByteToWrite Number of bytes to write to the EEPROM.
  * @retval EepromOperations value: EEPROM_STATUS_COMPLETE or EEPROM_STATUS_ERROR
  */
EepromOperations m95256_WriteBuffer(m95256_t eeprom,
    uint8_t * pBuffer,
    uint16_t WriteAddr,
    uint16_t NumByteToWrite);
/**
  * @brief  Reads a block of data from the EEPROM.
  *
  * @param	eeprom EEPROM instance handle
  * @param  pBuffer Pointer to the buffer that receives the data read from the EEPROM.
  * @param  ReadAddr EEPROM's internal address to read from.
  * @param  NumByteToRead Number of bytes to read from the EEPROM.
  * @retval EepromOperations value: EEPROM_STATUS_COMPLETE or EEPROM_STATUS_TIMEOUT
  */
EepromOperations m95256_ReadBuffer(m95256_t eeprom,
    uint8_t * pBuffer,
    uint16_t ReadAddr,
    uint16_t NumByteToRead);
/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the EEPROM's
  *         status register and loop until write operation has completed.
  *
  * @param	eeprom EEPROM instance handle
  * @retval EepromOperations value: EEPROM_STATUS_PENDING or EEPROM_STATUS_TIMEOUT
  */
EepromOperations m95256_WaitStandbyState(m95256_t eeprom);


/* Low layer functions */
/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  *
  * @param  byte Byte to send.
  * @retval The value of the received byte.
  */
uint8_t m95256_SendByte(SPI_HandleTypeDef * spi, uint8_t byte);
/**
  * @brief  Enables the write access to the EEPROM.
  *
  * @param	eeprom EEPROM instance handle
  */
void sEE_WriteEnable(m95256_t eeprom);
/**
  * @brief  Disables the write access to the EEPROM.
  *
  * @param	eeprom EEPROM instance handle
  */
void sEE_WriteDisable(m95256_t eeprom);

/**
  * @brief  Write new value in EEPROM Status Register.
  *
  * @param	eeprom EEPROM instance handle
  * @param  regval New value of register
  */
void sEE_WriteStatusRegister(m95256_t eeprom, uint8_t regval);
//uint8_t sEE_ReadStatusRegister(void);

/**
 * @brief Low level function to send header data to EEPROM
 *
 * @param spi Pointer to SPI struct handle
 * @param instruction Array of bytes to send
 * @param size Data size in bytes
 */
void m95256_SendInstruction(SPI_HandleTypeDef * spi, uint8_t * instruction, uint8_t size);
// void m95256_ReadStatusByte(SPI_HandleTypeDef SPIe, uint8_t *statusByte);

#ifdef __cplusplus
}
#endif

#endif