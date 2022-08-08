/**
 * @file		config.c
 * @brief		This file contains the configuration handler
 *
 * @date		May 28, 2021
 *
 * @author		Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "eeprom-config.h"

#include "spi.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

static m95256_t eeprom = NULL;

bool EEPROM_config_init(EEPROM_ConfigTypeDef *config, SPI_HandleTypeDef *eeprom_hspi, GPIO_TypeDef *eeprom_cs_gpio, uint16_t eeprom_cs_pin, uint16_t address, uint32_t version, void *default_data, size_t size) {
    assert(size + CONFIG_VERSION_SIZE <= EEPROM_BUFFER_SIZE);

    config->address = address;
    config->version = version;
    config->size    = size;
    config->dirty   = false;

    if (eeprom == NULL) {
        m95256_init(&eeprom, eeprom_hspi, eeprom_cs_gpio, eeprom_cs_pin);
    }

    if (EEPROM_config_read(config)) {
        return true;
    } else {
        // Data in eeprom is gibberish
        EEPROM_config_set(config, default_data);
    }
    return false;
}

bool EEPROM_config_read(EEPROM_ConfigTypeDef *config) {
    uint8_t buffer[EEPROM_BUFFER_SIZE] = {0};

    if (m95256_ReadBuffer(eeprom, buffer, config->address, config->size + CONFIG_VERSION_SIZE) ==
        EEPROM_STATUS_COMPLETE) {
        // Check if EEPROM's version matches config's
        if (*((CONFIG_VERSION_TYPE *)buffer) == config->version) {
            memcpy(config->data, buffer + CONFIG_VERSION_SIZE, config->size);
            config->dirty = false;

            return true;
        }
    }
    return false;
}

bool EEPROM_config_write(EEPROM_ConfigTypeDef *config) {
    if (config->dirty) {
        uint8_t buffer[EEPROM_BUFFER_SIZE] = {0};
        memcpy(buffer, &(config->version), CONFIG_VERSION_SIZE);           // Copy version
        memcpy(buffer + CONFIG_VERSION_SIZE, config->data, config->size);  // Copy data after version

        if (m95256_WriteBuffer(eeprom, buffer, config->address, config->size + CONFIG_VERSION_SIZE) ==
            EEPROM_STATUS_COMPLETE) {

            // Read just-written data and compare for errors
            uint8_t testbuf[EEPROM_BUFFER_SIZE] = {0};
            if (m95256_ReadBuffer(eeprom, testbuf, config->address, config->size + CONFIG_VERSION_SIZE) ==
                EEPROM_STATUS_COMPLETE) {
                if (memcmp(buffer, testbuf, CONFIG_VERSION_SIZE) == 0) {
                    config->dirty = false;
                    return true;
                }
                return false;
            }

        } else {
            return false;
        }
    }
    return true;
}

void *EEPROM_config_get(EEPROM_ConfigTypeDef *config) {
    return config->data;
}

void EEPROM_config_set(EEPROM_ConfigTypeDef *config, void *data) {
    memcpy(config->data, data, config->size);  // Copy data
    config->dirty = true;
}