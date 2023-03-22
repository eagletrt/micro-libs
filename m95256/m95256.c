/**
 * Copyright Nikita Bulaev 2017-2019
 *
 * Some parts of this lib is taken from STM32 StdPerif libriary
 * stm32l152d_eval_spi_ee.c and adopted for the HAL.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "m95256.h"

#include <stdlib.h>
#include "spi.h"

uint8_t EEPROM_StatusByte;
uint8_t RxBuffer[EEPROM_BUFFER_SIZE] = { 0x00 };

void m95256_init(m95256_t * eeprom,
    SPI_HandleTypeDef * spi,
    GPIO_TypeDef * cs_gpio,
    uint16_t cs_pin) {
    *eeprom            = malloc(sizeof(struct m95256));
    (*eeprom)->spi     = spi;
    (*eeprom)->cs_gpio = cs_gpio;
    (*eeprom)->cs_pin  = cs_pin;
}
void m95256_deinit(m95256_t * eeprom) {
    free(*eeprom);
}

EepromOperations m95256_WritePage(m95256_t eeprom,
    uint8_t * pBuffer,
    uint16_t WriteAddr,
    uint16_t NumByteToWrite) {
    while (eeprom->spi->State != HAL_SPI_STATE_READY)
        ;

    HAL_StatusTypeDef spiTransmitStatus;

    sEE_WriteEnable(eeprom);

    /*
        We gonna send commands in one packet of 3 bytes
     */
    uint8_t header[3];

    header[0] = EEPROM_WRITE;    // Send "Write to Memory" instruction
    header[1] = WriteAddr >> 8;  // Send 16-bit address
    header[2] = WriteAddr;

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW(eeprom->cs_gpio, eeprom->cs_pin);

    m95256_SendInstruction(eeprom->spi, (uint8_t *)header, 3);

    // Make 5 attemtps to write the data
    for (uint8_t i = 0; i < 5; i++) {
        spiTransmitStatus = HAL_SPI_Transmit(eeprom->spi, pBuffer, NumByteToWrite, 100);

        if (spiTransmitStatus == HAL_BUSY) {
            HAL_Delay(5);
        } else {
            break;
        }
    }

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH(eeprom->cs_gpio, eeprom->cs_pin);

    // Wait the end of EEPROM writing
    m95256_WaitStandbyState(eeprom);

    // Disable the write access to the EEPROM
    sEE_WriteDisable(eeprom);

    if (spiTransmitStatus == HAL_ERROR) {
        return EEPROM_STATUS_ERROR;
    } else {
        return EEPROM_STATUS_COMPLETE;
    }
}
EepromOperations m95256_WriteBuffer(m95256_t eeprom,
    uint8_t * pBuffer,
    uint16_t WriteAddr,
    uint16_t NumByteToWrite) {
    uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
    uint16_t sEE_DataNum = 0;

    EepromOperations pageWriteStatus = EEPROM_STATUS_PENDING;

    Addr        = WriteAddr % EEPROM_PAGESIZE;
    count       = EEPROM_PAGESIZE - Addr;
    NumOfPage   = NumByteToWrite / EEPROM_PAGESIZE;
    NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;

    if (Addr == 0) {          /* WriteAddr is EEPROM_PAGESIZE aligned  */
        if (NumOfPage == 0) { /* NumByteToWrite < EEPROM_PAGESIZE */
            sEE_DataNum     = NumByteToWrite;
            pageWriteStatus = m95256_WritePage(eeprom, pBuffer, WriteAddr, sEE_DataNum);

            if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                return pageWriteStatus;
            }

        } else { /* NumByteToWrite > EEPROM_PAGESIZE */
            while (NumOfPage--) {
                sEE_DataNum     = EEPROM_PAGESIZE;
                pageWriteStatus = m95256_WritePage(eeprom, pBuffer, WriteAddr, sEE_DataNum);

                if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                    return pageWriteStatus;
                }

                WriteAddr += EEPROM_PAGESIZE;
                pBuffer += EEPROM_PAGESIZE;
            }

            sEE_DataNum     = NumOfSingle;
            pageWriteStatus = m95256_WritePage(eeprom, pBuffer, WriteAddr, sEE_DataNum);

            if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                return pageWriteStatus;
            }
        }
    } else {                           /* WriteAddr is not EEPROM_PAGESIZE aligned  */
        if (NumOfPage == 0) {          /* NumByteToWrite < EEPROM_PAGESIZE */
            if (NumOfSingle > count) { /* (NumByteToWrite + WriteAddr) > EEPROM_PAGESIZE */
                temp            = NumOfSingle - count;
                sEE_DataNum     = count;
                pageWriteStatus = m95256_WritePage(eeprom, pBuffer, WriteAddr, sEE_DataNum);

                if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                    return pageWriteStatus;
                }

                WriteAddr += count;
                pBuffer += count;

                sEE_DataNum     = temp;
                pageWriteStatus = m95256_WritePage(eeprom, pBuffer, WriteAddr, sEE_DataNum);
            } else {
                sEE_DataNum     = NumByteToWrite;
                pageWriteStatus = m95256_WritePage(eeprom, pBuffer, WriteAddr, sEE_DataNum);
            }

            if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                return pageWriteStatus;
            }
        } else { /* NumByteToWrite > EEPROM_PAGESIZE */
            NumByteToWrite -= count;
            NumOfPage   = NumByteToWrite / EEPROM_PAGESIZE;
            NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;

            sEE_DataNum = count;

            pageWriteStatus = m95256_WritePage(eeprom, pBuffer, WriteAddr, sEE_DataNum);

            if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                return pageWriteStatus;
            }

            WriteAddr += count;
            pBuffer += count;

            while (NumOfPage--) {
                sEE_DataNum = EEPROM_PAGESIZE;

                pageWriteStatus = m95256_WritePage(eeprom, pBuffer, WriteAddr, sEE_DataNum);

                if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                    return pageWriteStatus;
                }

                WriteAddr += EEPROM_PAGESIZE;
                pBuffer += EEPROM_PAGESIZE;
            }

            if (NumOfSingle != 0) {
                sEE_DataNum = NumOfSingle;

                pageWriteStatus = m95256_WritePage(eeprom, pBuffer, WriteAddr, sEE_DataNum);

                if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                    return pageWriteStatus;
                }
            }
        }
    }

    return EEPROM_STATUS_COMPLETE;
}
EepromOperations m95256_ReadBuffer(m95256_t eeprom,
    uint8_t * pBuffer,
    uint16_t ReadAddr,
    uint16_t NumByteToRead) {
    uint32_t timestamp = HAL_GetTick();
    while (eeprom->spi->State != HAL_SPI_STATE_READY) {
        if (HAL_GetTick() - timestamp >= EEPROM_OPS_TIMEOUT) {
            return EEPROM_STATUS_TIMEOUT;
        }
    }

    // We gonna send all commands in one packet of 3 bytes
    uint8_t header[3];

    header[0] = EEPROM_READ;    // Send "Read from Memory" instruction
    header[1] = ReadAddr >> 8;  // Send 16-bit address
    header[2] = ReadAddr;

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW(eeprom->cs_gpio, eeprom->cs_pin);

    /* Send WriteAddr address byte to read from */
    m95256_SendInstruction(eeprom->spi, header, 3);

    timestamp = HAL_GetTick();
    while (HAL_SPI_Receive(eeprom->spi, (uint8_t *)pBuffer, NumByteToRead, 200) == HAL_BUSY) {
        if (HAL_GetTick() - timestamp >= EEPROM_OPS_TIMEOUT) {
            return EEPROM_STATUS_TIMEOUT;
        }
    };

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH(eeprom->cs_gpio, eeprom->cs_pin);

    return EEPROM_STATUS_COMPLETE;
}
EepromOperations m95256_WaitStandbyState(m95256_t eeprom) {
    uint8_t sEEstatus[1] = {0x00};
    uint8_t command[1]   = {EEPROM_RDSR};

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW(eeprom->cs_gpio, eeprom->cs_pin);

    // Send "Read Status Register" instruction
    m95256_SendInstruction(eeprom->spi, (uint8_t *)command, 1);

    uint32_t timestamp = HAL_GetTick();
    // Loop as long as the memory is busy with a write cycle
    do {
        while (HAL_SPI_Receive(eeprom->spi, (uint8_t *)sEEstatus, 1, 200) == HAL_BUSY) {
            if (HAL_GetTick() - timestamp >= EEPROM_OPS_TIMEOUT) {
                return EEPROM_STATUS_TIMEOUT;
            }
        };

        if (HAL_GetTick() - timestamp >= EEPROM_OPS_TIMEOUT) {
            return EEPROM_STATUS_TIMEOUT;
        }
    } while ((sEEstatus[0] & EEPROM_WIP_FLAG) == SET);  // Write in progress

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH(eeprom->cs_gpio, eeprom->cs_pin);

    return EEPROM_STATUS_PENDING;
}
uint8_t EEPROM_SendByte(SPI_HandleTypeDef * spi, uint8_t byte) {
    uint8_t answerByte;

    /* Loop while DR register in not empty */
    uint32_t timestamp = HAL_GetTick();
    while (spi->State == HAL_SPI_STATE_RESET) {
        if (HAL_GetTick() - timestamp >= EEPROM_OPS_TIMEOUT) {
            return EEPROM_STATUS_TIMEOUT;
        }
    }

    /* Send byte through the SPI peripheral */
    timestamp = HAL_GetTick();
    if (HAL_SPI_Transmit(spi, &byte, 1, 200) != HAL_OK) {
        return EEPROM_STATUS_ERROR;
    }

    /* Wait to receive a byte */
    timestamp = HAL_GetTick();
    while (spi->State == HAL_SPI_STATE_RESET) {
        if (HAL_GetTick() - timestamp >= EEPROM_OPS_TIMEOUT) {
            return EEPROM_STATUS_TIMEOUT;
        }
    }

    /* Return the byte read from the SPI bus */
    timestamp = HAL_GetTick();
    if (HAL_SPI_Receive(spi, &answerByte, 1, 200) != HAL_OK) {
        return EEPROM_STATUS_ERROR;
    }

    return (uint8_t)answerByte;
}
void sEE_WriteEnable(m95256_t eeprom) {
    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW(eeprom->cs_gpio, eeprom->cs_pin);

    uint8_t command[1] = {EEPROM_WREN};
    /* Send "Write Enable" instruction */
    m95256_SendInstruction(eeprom->spi, (uint8_t *)command, 1);

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH(eeprom->cs_gpio, eeprom->cs_pin);
}
void sEE_WriteDisable(m95256_t eeprom) {
    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW(eeprom->cs_gpio, eeprom->cs_pin);

    uint8_t command[1] = {EEPROM_WRDI};

    /* Send "Write Disable" instruction */
    m95256_SendInstruction(eeprom->spi, (uint8_t *)command, 1);

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH(eeprom->cs_gpio, eeprom->cs_pin);
}

void sEE_WriteStatusRegister(m95256_t eeprom, uint8_t regval) {
    uint8_t command[2];

    command[0] = EEPROM_WRSR;
    command[1] = regval;

    // Enable the write access to the EEPROM
    sEE_WriteEnable(eeprom);

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW(eeprom->cs_gpio, eeprom->cs_pin);

    // Send "Write Status Register" instruction
    // and Regval in one packet
    m95256_SendInstruction(eeprom->spi, (uint8_t *)command, 2);

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH(eeprom->cs_gpio, eeprom->cs_pin);

    sEE_WriteDisable(eeprom);
}

void m95256_SendInstruction(SPI_HandleTypeDef * spi, uint8_t * instruction, uint8_t size) {
    while (spi->State == HAL_SPI_STATE_RESET)
        ;

    if (HAL_SPI_Transmit(spi, (uint8_t *)instruction, (uint16_t)size, 200) != HAL_OK) {
        Error_Handler();
    }
}
