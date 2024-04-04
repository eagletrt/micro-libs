/**
 * @file ltc6811.c
 * @brief This file contains the functions to communicate with the LTC6811 chip
 * 
 * @attention In a daisy-chain configuration only broadcast commands are supported
 * For SPI communication with the LTC, mode 3 is required (in CubeMX is CPOL="Low" and CPHA="2 edge")
 * and the MISO pin has to be set in Pull-Up mode (if it's not set on the hardware side)
 * Maximum SPI communication speed with the LTC is 1MHz
 * 
 * @details For additional information refer to the datasheet:
 * @link https://www.analog.com/media/en/technical-documentation/data-sheets/ltc6811-1-6811-2.pdf
 *
 * @date 16 Dec 2023
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 */

#include "ltc6811.h"

#include <string.h>


/** @brief Table used to calculate the pec for messaging */
static const uint16_t crcTable[] = {
    0x0000, 0xc599, 0xceab, 0x0b32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd,
    0xf407, 0x319e, 0x3aac, 0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa,
    0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1, 0xbbf3, 0x7e6a,
    0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d,
    0x5b2e, 0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3,
    0xaf29, 0x6ab0, 0x6182, 0xa41b, 0x77e6, 0xb27f, 0xb94d, 0x7cd4,
    0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd, 0x2544,
    0x2be,  0xc727, 0xcc15, 0x98c,  0xda71, 0x1fe8, 0x14da, 0xd143,
    0xf3c5, 0x365c, 0x3d6e, 0xf8f7, 0x2b0a, 0xee93, 0xe5a1, 0x2038,
    0x7c2,  0xc25b, 0xc969, 0xcf0,  0xdf0d, 0x1a94, 0x11a6, 0xd43f,
    0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
    0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8,
    0xa8eb, 0x6d72, 0x6640, 0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16,
    0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba, 0x4a88, 0x8f11,
    0x57c,  0xc0e5, 0xcbd7, 0xe4e,  0xddb3, 0x182a, 0x1318, 0xd681,
    0xf17b, 0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286,
    0xa213, 0x678a, 0x6cb8, 0xa921, 0x7adc, 0xbf45, 0xb477, 0x71ee,
    0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070, 0x85e9,
    0xf84,  0xca1d, 0xc12f, 0x4b6,  0xd74b, 0x12d2, 0x19e0, 0xdc79,
    0xfb83, 0x3e1a, 0x3528, 0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e,
    0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59, 0x2ac0,
    0xd3a,  0xc8a3, 0xc391, 0x608,  0xd5f5, 0x106c, 0x1b5e, 0xdec7,
    0x54aa, 0x9133, 0x9a01, 0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757,
    0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9, 0x7350,
    0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b,
    0xa5d1, 0x6048, 0x6b7a, 0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c,
    0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25, 0x2fbc,
    0x846,  0xcddf, 0xc6ed, 0x374,  0xd089, 0x1510, 0x1e22, 0xdbbb,
    0xaf8,  0xcf61, 0xc453, 0x1ca,  0xd237, 0x17ae, 0x1c9c, 0xd905,
    0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b, 0x2d02,
    0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492,
    0x5368, 0x96f1, 0x9dc3, 0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
};

/**
 * @brief Generate the Packet Error Code (PEC) from an array of bytes
 * 
 * @param data The array of bytes
 * @param len The length of the array in bytes
 * @return uint16_t The calculated PEC
 */
static uint16_t _ltc6811_pec15(uint8_t * data, size_t len) {
    uint16_t remainder, address;
    remainder = 16;  // PEC seed
    for (int i = 0; i < len; i++) {
        // calculate PEC table address
        address = ((remainder >> 7) ^ data[i]) & 0xff;
        remainder = (remainder << 8) ^ crcTable[address];
    }
    // The CRC15 has a 0 in the LSB so the final value must be multiplied by 2
    return (remainder * 2);
}
/**
 * @brief Calculate and add pec to the 'data' array
 * @attention The array should be large enough to contains the 2 bytes PEC
 * 
 * @example data[8] => len = 6
 * 
 * @param data The array of data
 * @param len The length of the data in bytes
 */
static inline void _ltc6811_pec_calc(uint8_t * data, size_t len) {
    uint16_t pec = _ltc6811_pec15(data, len);
    data[len] = (uint8_t)(pec >> 8);
    data[len + 1] = (uint8_t)(pec);
}

/**
 * @brief Check if the received PEC is correct 
 * 
 * @param data The array of data
 * @param len Length of the data array in bytes, PEC included
 * @return true If the received PEC and the calculated one corresponds
 * @return false Otherwise
 */
static inline bool _ltc6811_pec_check(uint8_t * data, size_t len) {
    return _ltc6811_pec15(data, len - LTC6811_PEC_BYTE_COUNT) == (((uint16_t)data[len - 2] << 8) | (uint16_t)data[len - 1]);
}

/**
 * @brief Set the ADC conversion mode option inside the command
 *
 * @param cmd The command to modify
 * @param mode The ADC conversion mode
 * @return Ltc6811Command The modified command
 */
static inline Ltc6811Command _ltc6811_cmd_set_md(Ltc6811Command cmd, Ltc6811Md mode) {
    return cmd | (mode << 7);
}

/**
 * @brief Set the Pull-up/Pull-down current for open-wire conversion option inside the command
 *
 * @param cmd The command to modify
 * @param pup The Pull-up/Pull-down option
 * @return Ltc6811Command The modified command
 */
static inline Ltc6811Command _ltc6811_cmd_set_pup(Ltc6811Command cmd, Ltc6811Pup pup) {
    return cmd | (pup << 6);
}
/**
 * @brief Set the self test mode option inside the command
 *
 * @param cmd The command to modify
 * @param mode The self test mode option
 * @return Ltc6811Command The modified command
 */
static inline Ltc6811Command _ltc6811_cmd_set_st(Ltc6811Command cmd, Ltc6811St mode) {
    return cmd | (mode << 5);
}
/**
 * @brief Set the dischare permitted option inside the command
 *
 * @param cmd The command to modify
 * @param dcp The discharge permitted option
 * @return Ltc6811Command The modified command
 */
static inline Ltc6811Command _ltc6811_cmd_set_dcp(Ltc6811Command cmd, Ltc6811Dcp dcp) {
    return cmd | (dcp << 4);
}
/**
 * @brief Set the cell selection for ADC conversion inside the command
 *
 * @param cmd The command to modify
 * @param cells The selected cells
 * @return Ltc6811Command The modified command
 */
static inline Ltc6811Command _ltc6811_cmd_set_ch(Ltc6811Command cmd, Ltc6811Ch cells) {
    return cmd | cells;
}
/**
 * @brief Set the GPIOs selection for ADC conversion inside the command
 *
 * @param cmd The command to modify
 * @param gpios The selected GPIOs
 * @return Ltc6811Command The modified command
 */
static inline Ltc6811Command _ltc6811_cmd_set_chg(Ltc6811Command cmd, Ltc6811Chg gpios) {
    return cmd | gpios;
}
/**
 * @brief Set the status group selection for ADC conversion inside the command
 *
 * @param cmd The command to modify
 * @param groups The selected status groups
 * @return Ltc6811Command The modified command
 */
static inline Ltc6811Command _ltc6811_cmd_set_chst(Ltc6811Command cmd, Ltc6811Chst groups) {
    return cmd | groups;
}

/**
 * @brief Save command and calculated pec in the 'buf' array
 * 
 * @attention The 'buf' array has to be at least 4 bytes long,
 * only the first 4 bytes of the array are modified
 * 
 * @param cmd The LTC6811 command
 * @param is_address True if the address should be used, false if broadcast
 * @param address The address of the LTC6811
 * @param out The array where the command is written
 */
static inline void _ltc6811_cmd_encode(
    Ltc6811Command cmd,
    bool is_address,
    uint8_t address,
    uint8_t * out)
{
    out[0] = (uint8_t)((cmd >> 8) & 0x07);
    if (is_address) out[0] |= 0x80 | ((address & 0x0F) << 3);
    out[1] = (uint8_t)(cmd);
    _ltc6811_pec_calc(out, 2);
}

void ltc6811_chain_init(Ltc6811Chain * chain, size_t ltc_count) {
    if (chain == NULL)
        return;
    chain->count = ltc_count;
}

bool ltc6811_pladc_check(uint8_t byte) {
    return byte == 0xFF;
}

size_t ltc6811_wrcfg_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Cfgr * config,
    uint8_t * out)
{
    if (chain == NULL || config == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = WRCFGA;

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    size_t encoded = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;

    // Encode data with corresponsing PEC
    for (size_t i = 0; i < chain->count; ++i) {
        Ltc6811Cfgr * cfg = &(config[chain->count - i - 1]);

        // Copy data
        out[encoded] = (cfg->GPIO << 3) | (cfg->REFON << 2) | (cfg->DTEN << 1) | (cfg->ADCOPT);
        out[encoded + 1] = cfg->VUV;
        out[encoded + 2] = (cfg->VOV << 4) | (cfg->VUV >> 8);
        out[encoded + 3] = cfg->VOV >> 4;
        out[encoded + 4] = cfg->DCC;
        out[encoded + 5] = (cfg->DCTO << 4) | (cfg->DCC >> 8);

        // Add PEC
        _ltc6811_pec_calc(out + encoded, LTC6811_REG_BYTE_COUNT);
        encoded += LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    }
    return encoded;
}

size_t ltc6811_rdcfg_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;
    
    // Get command
    Ltc6811Command cmd = RDCFGA;

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_rdcfg_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    Ltc6811Cfgr * out)
{
    if (chain == NULL || data == NULL || out == NULL)
        return 0U;

    size_t decoded = 0, off = 0;
    const size_t byte_count = LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < chain->count; ++i) {
        // Check PEC validity
        if (_ltc6811_pec_check(data + off, byte_count)) {
            // Decode configuration data
            out[i].ADCOPT = data[off] & 0x01;
            out[i].DTEN = (data[off] & 0x02) >> 1;
            out[i].REFON = (data[off] & 0x04) >> 2;
            out[i].GPIO = (data[off] & 0xF8) >> 3;
            out[i].VUV = data[off + 1] | (((uint16_t)data[off + 2] & 0x0F) << 8);
            out[i].VOV = ((data[off + 2] & 0xF0) >> 4) | ((uint16_t)data[off + 3] << 4);
            out[i].DCC = data[off + 4] | (((uint16_t)data[off + 5] & 0x0F) << 8);
            out[i].DCTO = (data[off + 5] & 0xF0) >> 4;

            decoded += byte_count;
        }
        off += byte_count;
    }
    return decoded;
}

size_t ltc6811_rdcv_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Cvxr reg,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = RDCVA;
    switch(reg) {
        case LTC6811_CVAR:
            cmd = RDCVA;
            break;
        case LTC6811_CVBR:
            cmd = RDCVB;
            break;
        case LTC6811_CVCR:
            cmd = RDCVC;
            break;
        case LTC6811_CVDR:
            cmd = RDCVD;
            break;
        default:
            cmd = RDCVA;
            break;
    }

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_rdcv_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint16_t * out)
{
    if (chain == NULL || data == NULL || out == NULL)
        return 0;

    size_t decoded = 0, off = 0;
    const size_t byte_count = LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < chain->count; ++i) {
        // Check PEC validity
        if (_ltc6811_pec_check(data + off, byte_count)) {
            // For each cell voltage in register
            for (size_t j = 0; j < LTC6811_REG_CELL_COUNT; ++j) {
                size_t cell_off = off + j * sizeof(uint16_t);
                out[i * LTC6811_REG_CELL_COUNT + j] = data[cell_off] | ((uint16_t)data[cell_off + 1] << 8);
            }
            decoded += byte_count;
        }
        off += byte_count;
    }
    return decoded;
}

size_t ltc6811_rdaux_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Avxr reg,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U; 

    // Get command
    Ltc6811Command cmd = RDAUXA;
    switch(reg) {
        case LTC6811_AVAR:
            cmd = RDAUXA;
            break;
        case LTC6811_AVBR:
            cmd = RDAUXB;
            break;
        default:
            cmd = RDAUXA;
            break;
    }

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_rdaux_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint16_t * out)
{
    if (chain == NULL || data == NULL || out == NULL)
        return 0U;
    
    size_t decoded = 0, off = 0;
    const size_t byte_count = LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < chain->count; ++i) {
        // Check PEC validity
        if (_ltc6811_pec_check(data + off, byte_count)) {
            // For each auxilary voltage in register
            for (size_t j = 0; j < LTC6811_REG_AUX_COUNT; ++j) {
                size_t cell_off = off + j * sizeof(uint16_t);
                out[i * LTC6811_REG_AUX_COUNT + j] = data[cell_off] | ((uint16_t)data[cell_off + 1] << 8);
            }
            decoded += byte_count;
        }
        off += byte_count;
    }
    return decoded;
}

size_t ltc6811_rdstat_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Stxr reg,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U; 

    // Get command
    Ltc6811Command cmd = RDSTATA;
    switch(reg) {
        case LTC6811_STAR:
            cmd = RDSTATA;
            break;
        case LTC6811_STBR:
            cmd = RDSTATB;
            break;
        default:
            cmd = RDSTATA;
            break;
    }

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_rdstat_decode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Stxr reg,
    uint8_t * data,
    Ltc6811Str * out)
{
    if (chain == NULL || data == NULL || out == NULL)
        return 0U;
    
    size_t decoded = 0, off = 0;
    const size_t byte_count = LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < chain->count; ++i) {
        // Check PEC validity
        if (_ltc6811_pec_check(data + off, byte_count)) {
            switch (reg) {
                case LTC6811_STAR:
                    out[i].SC   = data[off] | ((uint16_t)data[off + 1] << 8);
                    out[i].ITMP = data[off + 2] | ((uint16_t)data[off + 3] << 8);
                    out[i].VA   = data[off + 4] | ((uint16_t)data[off + 5] << 8);
                    break;
                case LTC6811_STBR:
                    out[i].VD = data[off] | ((uint16_t)data[off + 1] << 8);
                    // Get CUV and COV bits
                    for (size_t byte = 0; byte < 3; ++byte) {
                        for (size_t b = 0; b < 4; ++b) {
                            uint16_t cuv = (data[off + byte + 2] & (1 << (b * 2))) >> b;
                            uint16_t cov = (data[off + byte + 2] & (1 << (b * 2 + 1))) >> (b + 1);
                            out[i].CUV |= cuv << (byte * 4);
                            out[i].COV |= cov << (byte * 4);
                        }
                    }
                    out[i].REV     = (data[off + 5] & 0b11110000) >> 4;
                    out[i].RSVD    = (data[off + 5] & 0b00001100) >> 2;
                    out[i].MUXFAIL = (data[off + 5] & 0b00000010) >> 1;
                    out[i].THSD    = (data[off + 5] & 0b00000001);
                    break;
                default:
                    return 0U;
            }
            decoded += byte_count;
        }
        off += byte_count;
    }
    return decoded;
}

size_t ltc6811_wrsctrl_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint8_t * out)
{
    if (chain == NULL || data == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = WRSCTRL;

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    size_t encoded = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;

    // Encode data with corresponsing PEC
    for (size_t i = 0; i < chain->count; ++i) {
        for (size_t byte = 0; byte < LTC6811_REG_BYTE_COUNT; ++byte) {
            size_t index = i * LTC6811_SCTL_COUNT + byte * 2;
            out[encoded + byte] = (data[index] & 0x0F) | (data[index + 1] & 0x0F) << 4;
        }
        _ltc6811_pec_calc(out + encoded, LTC6811_REG_BYTE_COUNT);
        encoded += LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    }
    return encoded;
}

size_t ltc6811_rdsctrl_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = RDSCTRL;

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_rdsctrl_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint8_t * out)
{
    if (chain == NULL || data == NULL || out == NULL)
        return 0;

    size_t decoded = 0, off = 0;
    const size_t byte_count = LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < chain->count; ++i) {
        // Check PEC validity
        if (_ltc6811_pec_check(data + off, byte_count)) {
            // For each data byte
            for (size_t byte = 0; byte < LTC6811_REG_BYTE_COUNT; ++byte) {
                size_t index = i * LTC6811_SCTL_COUNT + byte * 2;
                out[index] = data[off + byte] & 0x0F;
                out[index + 1] = data[off + byte] >> 4;
            }
            decoded += byte_count;
        }
        off += byte_count;
    }
    return decoded;
}

size_t ltc6811_stsctrl_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = STSCTRL;

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_clrsctrl_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = CLRSCTRL;

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_wrpwm_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint8_t * out)
{
    if (chain == NULL || data == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = WRPWM;

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    size_t encoded = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;

    // Encode data with corresponsing PEC
    for (size_t i = 0; i < chain->count; ++i) {
        for (size_t byte = 0; byte < LTC6811_REG_BYTE_COUNT; ++byte) {
            size_t index = i * LTC6811_SCTL_COUNT + byte * 2;
            out[encoded + byte] = (data[index] & 0x0F) | (data[index + 1] & 0x0F) << 4;
        }
        _ltc6811_pec_calc(out + encoded, LTC6811_REG_BYTE_COUNT);
        encoded += LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    }
    return encoded;
}

size_t ltc6811_rdpwm_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = RDPWM;

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_rdpwm_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    uint8_t * out)
{
    if (chain == NULL || data == NULL || out == NULL)
        return 0;

    size_t decoded = 0, off = 0;
    const size_t byte_count = LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < chain->count; ++i) {
        // Check PEC validity
        if (_ltc6811_pec_check(data + off, byte_count)) {
            // For each data byte
            for (size_t byte = 0; byte < LTC6811_REG_BYTE_COUNT; ++byte) {
                size_t index = i * LTC6811_PWM_COUNT + byte * 2;
                out[index] = data[off + byte] & 0x0F;
                out[index + 1] = data[off + byte] >> 4;
            }
            decoded += byte_count;
        }
        off += byte_count;
    }
    return decoded;
}

size_t ltc6811_adcv_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Dcp dcp,
    Ltc6811Ch cells,
    uint8_t * out)
{ 
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = ADCV;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_dcp(cmd, dcp);
    cmd = _ltc6811_cmd_set_ch(cmd, cells);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_adow_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Pup pup,
    Ltc6811Dcp dcp,
    Ltc6811Ch cells,
    uint8_t * out)
{ 
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = ADOW;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_pup(cmd, mode);
    cmd = _ltc6811_cmd_set_dcp(cmd, dcp);
    cmd = _ltc6811_cmd_set_ch(cmd, cells);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_cvst_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811St test_mode,
    uint8_t * out)
{ 
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = CVST;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_st(cmd, test_mode);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_adol_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Dcp dcp,
    uint8_t * out)
{ 
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = ADOL;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_dcp(cmd, dcp);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_adax_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Chg gpios,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = ADAX;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_chg(cmd, gpios);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_adaxd_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Chg gpios,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = ADAXD;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_chg(cmd, gpios);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_axst_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811St test_mode,
    uint8_t * out)
{ 
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = AXST;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_st(cmd, test_mode);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_adstat_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Chst groups,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = ADSTAT;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_chst(cmd, groups);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_adstatd_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Chst groups,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = ADSTATD;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_chst(cmd, groups);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_statst_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811St test_mode,
    uint8_t * out)
{ 
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = STATST;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_st(cmd, test_mode);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_adcvax_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Dcp dcp,
    uint8_t * out)
{ 
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = ADCVAX;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_dcp(cmd, dcp);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_adcvsc_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Md mode,
    Ltc6811Dcp dcp,
    uint8_t * out)
{ 
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = ADCVSC;
    cmd = _ltc6811_cmd_set_md(cmd, mode);
    cmd = _ltc6811_cmd_set_dcp(cmd, dcp);

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_clrcell_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = CLRCELL;

    // Encode the command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_clraux_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = CLRAUX;

    // Encode the command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_clrstat_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = CLRSTAT;

    // Encode the command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_pladc_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = PLADC;

    // Encode the command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_diagn_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = DIAGN;

    // Encode the command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_wrcomm_encode_broadcast(
    Ltc6811Chain * chain,
    Ltc6811Comm * comm,
    uint8_t * out)
{
    if (chain == NULL || comm == NULL || out == NULL)
        return 0U;

    // Get command
    Ltc6811Command cmd = WRCOMM;

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    size_t encoded = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;

    // Encode data with corresponsing PEC
    for (size_t i = 0; i < chain->count; ++i) {
        Ltc6811Comm * data = &(comm[chain->count - i - 1]);

        // Copy data
        out[encoded] = (data->icom0 << 4) | (data->data[0] >> 4);
        out[encoded + 1] = ((data->data[0] & 0x0F) << 4) | data->fcom0;
        out[encoded + 2] = (data->icom1 << 4) | (data->data[1] >> 4);
        out[encoded + 3] = ((data->data[1] & 0x0F) << 4) | data->fcom1;
        out[encoded + 4] = (data->icom2 << 4) | (data->data[2] >> 4);
        out[encoded + 5] = ((data->data[2] & 0x0F) << 4) | data->fcom2;

        // Add PEC
        _ltc6811_pec_calc(out + encoded, LTC6811_REG_BYTE_COUNT);
        encoded += LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    }
    return encoded;
}

size_t ltc6811_rdcomm_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;
    
    // Get command
    Ltc6811Command cmd = RDCOMM;

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    return LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
}

size_t ltc6811_rdcomm_decode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * data,
    Ltc6811Comm * out)
{
    if (chain == NULL || data == NULL || out == NULL)
        return 0U;

    size_t decoded = 0, off = 0;
    const size_t byte_count = LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < chain->count; ++i) {
        // Check PEC validity
        if (_ltc6811_pec_check(data + off, byte_count)) {
            // Decode communication data
            out[i].icom0 = data[0] >> 4;
            out[i].icom1 = data[2] >> 4;
            out[i].icom2 = data[4] >> 4;
            out[i].fcom0 = data[1] & 0x0F;
            out[i].fcom1 = data[3] & 0x0F;
            out[i].fcom2 = data[5] & 0x0F;
            for (size_t j = 0; j < LTC6811_COMM_DATA_COUNT; ++j) 
                out[i].data[j] = ((data[j * 2] & 0x0F) << 4) | (data[j * 2 + 1] >> 4);

            decoded += byte_count;
        }
        off += byte_count;
    }
    return decoded;
}

size_t ltc6811_stcomm_encode_broadcast(
    Ltc6811Chain * chain,
    uint8_t * out)
{
    if (chain == NULL || out == NULL)
        return 0U;
    
    // Get command
    Ltc6811Command cmd = STCOMM;

    // Encode command with corresponding PEC
    _ltc6811_cmd_encode(cmd, false, 0U, out);
    size_t encoded = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    
    // Add dummy data
    memset(out + encoded, 0xFF, LTC6811_STCOMM_CYCLES);
    encoded += LTC6811_STCOMM_CYCLES;
    return encoded;
}

