/**
 * @file		ltc6811.c
 * @brief		This file contains the functions to communicate with the LTCs
 *
 * @date		Mar 15, 2023
 * @author		Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#include "ltc6811.h"

void ltc6811_enable_cs(SPI_HandleTypeDef * spi, GPIO_TypeDef * gpio, uint16_t pin) {
    HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_RESET);
}
void ltc6811_disable_cs(SPI_HandleTypeDef * spi, GPIO_TypeDef * gpio, uint16_t pin) {
    HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_SET);
}

void ltc6811_wakeup_idle(SPI_HandleTypeDef * spi, GPIO_TypeDef * gpio, uint16_t pin) {
    uint8_t data = 0xFF;

    ltc6811_enable_cs(spi, gpio, pin);
    HAL_SPI_Transmit(spi, &data, sizeof(data), 1);
    ltc6811_disable_cs(spi, gpio, pin);
}

void ltc6811_clrcell(SPI_HandleTypeDef * spi, GPIO_TypeDef * gpio, uint16_t pin) {
    uint8_t cmd[4];
    uint16_t pec;
    cmd[0] = 0b00000111;
    cmd[1] = 0b00010001;
    pec    = ltc6811_pec15(cmd, sizeof(pec));
    cmd[2] = (uint8_t)(pec >> 8);
    cmd[3] = (uint8_t)(pec);

    ltc6811_enable_cs(spi, gpio, pin);
    HAL_SPI_Transmit(spi, cmd, sizeof(cmd), 100);
    ltc6811_disable_cs(spi, gpio, pin);
}
void ltc6811_adcv(SPI_HandleTypeDef * spi,
    LTC6811_MD MD,
    LTC6811_DCP DCP,
    LTC6811_CH CH,
    GPIO_TypeDef * gpio,
    uint16_t pin) {
    uint8_t cmd[4];
    uint16_t pec;
    cmd[0] = 0b00000011 | (uint8_t)(MD >> 1);
    cmd[1] = 0b01100000 | (uint8_t)(MD << 7) | (uint8_t)(DCP << 3) | (uint8_t)CH;
    pec    = ltc6811_pec15(cmd, sizeof(pec));
    cmd[2] = (uint8_t)(pec >> 8);
    cmd[3] = (uint8_t)(pec);

    ltc6811_wakeup_idle(spi, gpio, pin);
    ltc6811_enable_cs(spi, gpio, pin);
    HAL_SPI_Transmit(spi, cmd, sizeof(cmd), 100);
    ltc6811_disable_cs(spi, gpio, pin);
}
void ltc6811_adow(SPI_HandleTypeDef * spi,
    LTC6811_PUP PUP,
    LTC6811_MD MD,
    LTC6811_DCP DCP,
    LTC6811_CH CH,
    GPIO_TypeDef * gpio,
    uint16_t pin) {
    uint8_t cmd[4];
    uint16_t pec;
    cmd[0] = 0b00000011 | (uint8_t)(MD >> 1);
    cmd[1] = 0b00101000 | (uint8_t)(MD << 7) | (uint8_t)(PUP << 6) | (uint8_t)(DCP << 3) | (uint8_t)CH;
    pec    = ltc6811_pec15(cmd, sizeof(pec));
    cmd[2] = (uint8_t)(pec >> 8);
    cmd[3] = (uint8_t)(pec);

    ltc6811_wakeup_idle(spi, gpio, pin);
    ltc6811_enable_cs(spi, gpio, pin);
    HAL_SPI_Transmit(spi, cmd, sizeof(cmd), 100);
    ltc6811_disable_cs(spi, gpio, pin);
}
void ltc6811_adax(SPI_HandleTypeDef * spi,
    LTC6811_MD MD,
    LTC6811_CHG CHG,
    GPIO_TypeDef * gpio,
    uint16_t pin) {
    uint8_t cmd[4];
    uint16_t pec;
    cmd[0] = 0b00000100 | (uint8_t)(MD >> 1);
    cmd[1] = 0b01100000 | (uint8_t)(MD << 7) | (uint8_t)CHG;
    pec    = ltc6811_pec15(cmd, sizeof(pec));
    cmd[2] = (uint8_t)(pec >> 8);
    cmd[3] = (uint8_t)(pec);

    ltc6811_wakeup_idle(spi, gpio, pin);
    ltc6811_enable_cs(spi, gpio, pin);
    HAL_SPI_Transmit(spi, cmd, sizeof(cmd), 100);
    ltc6811_disable_cs(spi, gpio, pin);
}
HAL_StatusTypeDef ltc6811_pladc(SPI_HandleTypeDef * spi, GPIO_TypeDef * gpio, uint16_t pin) {
    uint8_t cmd[4];
    uint16_t pec;
    cmd[0] = 0b00000111;
    cmd[1] = 0b00010100;
    pec    = ltc6811_pec15(cmd, sizeof(pec));
    cmd[2] = (uint8_t)(pec >> 8);
    cmd[3] = (uint8_t)(pec);

    ltc6811_wakeup_idle(spi, gpio, pin);
    ltc6811_enable_cs(spi, gpio, pin);
    HAL_SPI_Transmit(spi, cmd, sizeof(cmd), 100);

    uint8_t rx_data = 0;
    HAL_StatusTypeDef status =
        HAL_SPI_Receive(spi, &rx_data, sizeof(rx_data), 10);  // SDO pin is pulled down until the conversion is finished
    ltc6811_disable_cs(spi, gpio, pin);

    return status;
}

void ltc6811_wrcfg(SPI_HandleTypeDef * spi,
    LTC6811_WRCFG reg,
    uint8_t cfgr[8],
    GPIO_TypeDef * gpio,
    uint16_t pin) {
    uint8_t cmd[4] = {0};

    cmd[1]       = (reg == WRCFGA) ? 1 : 0b00100100;
    uint16_t pec = ltc6811_pec15(cmd, sizeof(pec));
    cmd[2]       = (uint8_t)(pec >> 8);
    cmd[3]       = (uint8_t)(pec);

    ltc6811_enable_cs(spi, gpio, pin);
    HAL_SPI_Transmit(spi, cmd, sizeof(cmd), 100);

    // Set the configuration for the 'i' ltc on the chain
    // GPIO configs are equal for all ltcs
    // CFGR[GPIO_CFGAR_POS] = GPIO_CONFIG + ((!GPIO_CFGAR_MASK) | cfgr[GPIO_CFGAR_POS]);
    HAL_SPI_Transmit(spi, cfgr, sizeof(cfgr), 100);
    ltc6811_disable_cs(spi, gpio, pin);
}

uint16_t ltc6811_pec15(uint8_t data[], uint8_t len) {
    uint16_t remainder, address;
    remainder = 16;  // PEC seed
    for (int i = 0; i < len; i++) {
        // calculate PEC table address
        address   = ((remainder >> 7) ^ data[i]) & 0xff;
        remainder = (remainder << 8) ^ crcTable[address];
    }
    // The CRC15 has a 0 in the LSB so the final value must be multiplied by 2
    return (remainder * 2);
}

HAL_StatusTypeDef ltc6811_read_voltages(SPI_HandleTypeDef * spi,
    voltage_t volts[LTC6811_CELL_COUNT],
    GPIO_TypeDef * gpio,
    uint16_t pin) {
    uint8_t cmd[4] = { 0 };
    uint16_t pec;
    uint8_t data[LTC6811_REG_CELL_COUNT * sizeof(voltage_t) + sizeof(pec)];
    HAL_StatusTypeDef status;

    // Start polling
    status = ltc6811_pladc(spi, gpio, pin);
    if (status == HAL_TIMEOUT)
        return HAL_TIMEOUT;

    // Auxiliary array used to iterate registers
    uint8_t rdcv[] = {
        RDCVA,
        RDCVB,
        RDCVC,
        RDCVD,
        RDCVE,
        RDCVF
    };

    // Read registers
    for (uint8_t reg = 0; reg < LTC6811_REG_COUNT; reg++) {
        cmd[1] = rdcv[reg];
        pec    = ltc6811_pec15(cmd, sizeof(pec));
        cmd[2] = (uint8_t)(pec >> 8);
        cmd[3] = (uint8_t)(pec);

        ltc6811_wakeup_idle(spi, gpio, pin);
        ltc6811_enable_cs(spi, gpio, pin);

        status = HAL_SPI_Transmit(spi, cmd, sizeof(cmd), 10);
        if (status != HAL_OK) {
            ltc6811_disable_cs(spi, gpio, pin);
            return status;
        }
        HAL_SPI_Receive(spi, data, LTC6811_REG_CELL_COUNT * sizeof(voltage_t) + sizeof(pec), 100);

        ltc6811_disable_cs(spi, gpio, pin);

        // Check pec validity
        if ( ltc6811_pec15(data, sizeof(data) - sizeof(pec)) == 
            (uint16_t)((data[6] << 8) | data[7]) ) {

            for (uint8_t cell = 0; cell < LTC6811_REG_CELL_COUNT; cell++) {
                uint8_t index = (reg * LTC6811_REG_CELL_COUNT) + cell;
                voltage_t * p_data = (voltage_t *)(data + sizeof(voltage_t) * cell);

                if (*p_data != 0xFFFF)
                    volts[index] = *p_data;
            }
        }
    }
    return status;
}

void ltc6811_build_dcc(uint32_t cells, uint8_t cfgr[8]) {
    // Auxiliary array used to iterate DCCs
    uint16_t dcc[] = {
        DCC1,  DCC2,  DCC3,
        DCC4,  DCC5,  DCC6,
        DCC7,  DCC8,  DCC9,
        DCC10, DCC11, DCC12
    };

    for (uint8_t i = 0; i < LTC6811_CELL_COUNT; ++i) {
        if (cells & (1 << i)) { // CANLIB_BITTEST
            if (i < 8)
                cfgr[4] |= dcc[i];
            else if (i < 12)
                cfgr[5] |= dcc[i];
        }
    }

    uint16_t pec = ltc6811_pec15(cfgr, sizeof(cfgr) - sizeof(pec));
    cfgr[6] = (uint8_t)(pec >> 8);
    cfgr[7] = (uint8_t)(pec);
}
void ltc6811_set_balancing(SPI_HandleTypeDef * spi,
    uint32_t cells,
    LTC6811_DCTO dcto,
    GPIO_TypeDef * gpio,
    uint16_t pin) {
    uint8_t cfgr[8] = { 0 };

    cfgr[0] |= (1 << 1);    // Set DTEN
    cfgr[5] |= (dcto << 4); // Set DCTO
    ltc6811_build_dcc(cells, cfgr);

    ltc6811_wakeup_idle(spi, gpio, pin);
    ltc6811_wrcfg(spi, WRCFGA, cfgr, gpio, pin);
}