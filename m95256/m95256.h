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
#define EEPROM_WREN 0x06  /*!< Write Enable */
#define EEPROM_WRDI 0x04  /*!< Write Disable */
#define EEPROM_RDSR 0x05  /*!< Read Status Register */
#define EEPROM_WRSR 0x01  /*!< Write Status Register */
#define EEPROM_READ 0x03  /*!< Read from Memory Array */
#define EEPROM_WRITE 0x02 /*!< Write to Memory Array */

#define EEPROM_WIP_FLAG 0x01 /*!< Write In Progress (WIP) flag */

#define EEPROM_PAGESIZE 32	  /*!< Pagesize according to documentation */
#define EEPROM_BUFFER_SIZE 32 /*!< EEPROM Buffer size. Setup to your needs */

#define EEPROM_CS_HIGH(GPIO, PIN) HAL_GPIO_WritePin(GPIO, PIN, GPIO_PIN_SET)
#define EEPROM_CS_LOW(GPIO, PIN) HAL_GPIO_WritePin(GPIO, PIN, GPIO_PIN_RESET)

typedef struct m95256* m95256_t;

/**
 * @brief EEPROM Operations statuses
 */
typedef enum {
	EEPROM_STATUS_PENDING,
	EEPROM_STATUS_COMPLETE,
	EEPROM_STATUS_ERROR
} EepromOperations;

void m95256_init(m95256_t* eeprom, SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_gpio, uint16_t cs_pin);
void m95256_deinit(m95256_t* eeprom);
EepromOperations m95256_WriteBuffer(m95256_t eeprom, uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
EepromOperations m95256_WritePage(m95256_t eeprom, uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
EepromOperations m95256_ReadBuffer(m95256_t eeprom, uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);
uint8_t m95256_WaitStandbyState(m95256_t eeprom);

/* Low layer functions */
uint8_t m95256_SendByte(SPI_HandleTypeDef* hspi, uint8_t byte);
void sEE_WriteEnable(m95256_t eeprom);
void sEE_WriteDisable(m95256_t eeprom);
void sEE_WriteStatusRegister(m95256_t eeprom, uint8_t regval);
//uint8_t sEE_ReadStatusRegister(void);

void m95256_SendInstruction(SPI_HandleTypeDef* hspi, uint8_t* instruction, uint8_t size);
void m95256_ReadStatusByte(SPI_HandleTypeDef SPIe, uint8_t* statusByte);

#ifdef __cplusplus
}
#endif

#endif