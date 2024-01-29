/**
 * @file volt.h
 * @brief Wrapper for voltage measurement with the LTC6811
 *
 * @date Mar 22, 2023
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#ifndef VOLT_H
#define VOLT_H

#include <inttypes.h>

#include "monitor_config.h"
#include "ltc6811.h"

/**
 * @brief Start voltage measurement
 * 
 * @param spi The spi configuration structure
 * @param MD The ADC conversion mode
 * @param DCP Discharge permitted option
 * @param CH Cell channel selection
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 */
void volt_start_measure(SPI_HandleTypeDef * spi,
    LTC6811_MD MD,
    LTC6811_DCP DCP,
    LTC6811_CH CH,
    GPIO_TypeDef * gpio,
    uint16_t pin);
/**
 * @brief Read cell voltages and save the values in the 'volts' array
 * 
 * @param spi The SPI configuration structure
 * @param volts An array where the result is stored
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 * @return HAL_StatusTypeDef The status of the operation
 */
HAL_StatusTypeDef volt_read(SPI_HandleTypeDef * spi,
    voltage_t volts[LTC6811_CELL_COUNT],
    GPIO_TypeDef * gpio,
    uint16_t pin);

/**
 * @brief Start open wire check
 * 
 * @param spi The SPI configuration structure
 * @param PUP Pull-Up/Pull-Down current for Open Wire conversion
 * @param MD The ADC conversion mode
 * @param DCP Discharge permitted option
 * @param gpio The GPIO port
 * @param pin The GPIO pin
 */
void volt_start_open_wire_check(SPI_HandleTypeDef * spi,
    LTC6811_PUP PUP,
    LTC6811_MD MD,
    LTC6811_DCP DCP,
    GPIO_TypeDef * gpio,
    uint16_t pin);
/**
 * @brief Check for open wire
 * 
 * @param pup An array containing voltage values read with PUP_ACTIVE (pull-up)
 * @param pud An array containing voltage values read with PUP_INACTIVE (pull-down)
 * @return HAL_StatusTypeDef Values: HAL_ERROR if open wire is detected, HAL_OK otherwise
 */
HAL_StatusTypeDef volt_open_wire_check(voltage_t pup[LTC6811_CELL_COUNT], voltage_t pud[LTC6811_CELL_COUNT]) {



#endif // VOLT_H