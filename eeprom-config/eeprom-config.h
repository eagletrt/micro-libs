/**
 * @file		config.h
 * @brief		This file contains the configuration handler
 *
 * @date		May 28, 2021
 *
 * @author		Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#ifndef _EEPROM_CONFIG_H_
#define _EEPROM_CONFIG_H_
#include "m95256.h"

#include <inttypes.h>
#include <stdbool.h>

#define CONFIG_VERSION_TYPE uint32_t
#define CONFIG_VERSION_SIZE sizeof(CONFIG_VERSION_TYPE)

typedef struct config {
    CONFIG_VERSION_TYPE version;
    uint16_t address;
    bool dirty;
    size_t size;
    uint8_t data[EEPROM_BUFFER_SIZE - CONFIG_VERSION_SIZE];
} config_t;

/**
 * @brief Initializes a config instance
 * 
 * @param config The config handle
 * @param address The handle to read/write the config to
 * @param version The version string to be used as comparison
 * @param default_data The default value for data. Used when the config in eeprom is invalid
 * @param size Size of the data structure
 * @return true Initialization from EEPROM successful
 * @return false The default config has been initialized/error reading EEPROM
 */
bool config_init(config_t *config, SPI_HandleTypeDef *eeprom_hspi, GPIO_TypeDef *eeprom_cs_gpio, uint16_t eeprom_cs_pin, uint16_t address, uint32_t version, void *default_data, size_t size);

/**
 * @brief Writes a config to EEPROM
 * 
 * @param config The config handle
 * @return true Write successfull
 * @return false Error
 */
bool config_write(config_t *config);

/**
 * @brief Reads a config from EEPROM
 * 
 * @param config The config handle
 * @return true Read successfull
 * @return false Error
 */
bool config_read(config_t *config);

/**
 * @brief Returns the config data
 * 
 * @note This function doesn NOT read from EEPROM. call config_read for that functionality
 * 
 * @param config The config handle
 * @return void* The data
 */
void *config_get(config_t *config);

/**
 * @brief Writes in the data register of the config handle
 * 
 * @note This function doesn NOT write to EEPROM. call config_write for that functionality
 * 
 * @param config The config handle
 * @param data data to write
 */
void config_set(config_t *config, void *data);

#endif