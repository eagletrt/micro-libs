/**
 * @file volt.c
 * @brief Wrapper for voltage measurement with the LTC6811
 *
 * @date Mar 22, 2023
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#include "volt.h"
#include "ltc6811.h"

void volt_start_measure(SPI_HandleTypeDef * spi,
    LTC6811_MD MD,
    LTC6811_DCP DCP,
    LTC6811_CH CH,
    GPIO_TypeDef * gpio,
    uint16_t pin) {
    ltc6811_adcv(spi, MD, DCP, CH, gpio, pin);
}

HAL_StatusTypeDef volt_read(SPI_HandleTypeDef * spi,
    voltage_t volts[LTC6811_CELL_COUNT],
    GPIO_TypeDef * gpio,
    uint16_t pin) {
    return ltc6811_read_voltages(spi, volts, gpio, pin);
}

void volt_start_open_wire_check(SPI_HandleTypeDef * spi,
    LTC6811_PUP PUP,
    LTC6811_MD MD,
    LTC6811_DCP DCP,
    GPIO_TypeDef * gpio,
    uint16_t pin) {
    ltc6811_adow(spi, PUP, MD, DCP, CELL_CH_ALL, gpio, pin);
}

HAL_StatusTypeDef volt_open_wire_check(voltage_t pup[LTC6811_CELL_COUNT], voltage_t pud[LTC6811_CELL_COUNT]) {
    // Check open wire on C(0) and C(12)
    if (pup[0] == 0 || pud[LTC6811_CELL_COUNT - 1] == 0)
        return HAL_ERROR;
    
    // Check open wire from C(1) to C(11)
    int32_t delta;
    for (uint8_t i = 1; i < LTC6811_CELL_COUNT; i++) {
        delta = (int32_t) pup[i] - pud[i];
        if (delta < -400)   // -400 mV
            return HAL_ERROR;
    }
    return HAL_OK;
}