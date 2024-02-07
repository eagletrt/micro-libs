/**
 * @file test-ltc6811.c
 * @brief Unit test for LTC6811 firmware
 *
 * @date 03 Gen 2024
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 */

#include "unity.h"
#include "ltc6811.h"

#define LTC_COUNT 3

uint16_t cmd_pecs[] = {
    [WRCFGA  ] = 0x3d6e,
    [WRCFGB  ] = 0xb19e,
    [RDCFGA  ] = 0x2b0a,
    [RDCFGB  ] = 0x2cc8,
    [RDCVA   ] = 0x07c2,
    [RDCVB   ] = 0x9a94,
    [RDCVC   ] = 0x5e52,
    [RDCVD   ] = 0xc304,
    [RDCVE   ] = 0xd560,
    [RDCVF   ] = 0x4836,
    [RDAUXA  ] = 0xefcc,
    [RDAUXB  ] = 0x729a,
    [RDAUXC  ] = 0x64fe,
    [RDAUXD  ] = 0xf9a8,
    [RDSTATA ] = 0xed72,
    [RDSTATB ] = 0x7024,
    [WRSCTRL ] = 0x5cec,
    [WRPWM   ] = 0x0000,
    [WRPSB   ] = 0xb4e2,
    [RDSCTRL ] = 0xc1ba,
    [RDPWM   ] = 0x9d56,
    [RDPSB   ] = 0x29b4,
    [STSCTRL ] = 0x8e4e,
    [CLRSCTRL] = 0x057c,
    [ADCV    ] = 0x7c20,
    [ADOW    ] = 0x73a4,
    [CVST    ] = 0x8a0c,
    [ADOL    ] = 0xa6c4,
    [ADAX    ] = 0x5bec,
    [ADAXD   ] = 0x0a3a,
    [AXST    ] = 0xadc0,
    [ADSTAT  ] = 0xb3e2,
    [ADSTATD ] = 0xe234,
    [STATST  ] = 0x45ce,
    [ADCVAX  ] = 0x1418,
    [ADCVSC  ] = 0xfc16,
    [CLRCELL ] = 0xc9c0,
    [CLRAUX  ] = 0xdfa4,
    [CLRSTAT ] = 0x5496,
    [PLADC   ] = 0xf36c,
    [DIAGN   ] = 0x785e,
    [WRCOMM  ] = 0x24b2,
    [RDCOMM  ] = 0x32d6,
    [STCOMM  ] = 0xb9e4
};

Ltc6811Cfgr config[LTC_COUNT];
uint8_t config_bytes[LTC_COUNT][sizeof(Ltc6811Cfgr)];
uint8_t config_pec_bytes[LTC_COUNT][2];

Ltc6811Str status[LTC_COUNT];

uint8_t sctrl[LTC6811_SCTL_COUNT * LTC_COUNT];
uint8_t sctrl_bytes[LTC_COUNT][LTC6811_REG_BYTE_COUNT];
uint8_t sctrl_pec_bytes[LTC_COUNT][2];

Ltc6811Comm comm[LTC_COUNT];
uint8_t comm_bytes[LTC_COUNT][sizeof(Ltc6811Comm)];
uint8_t comm_pec_bytes[LTC_COUNT][2];

Ltc6811Chain chain;

void setUp(void) {
    ltc6811_chain_init(&chain, LTC_COUNT);

    for (size_t i = 0; i < LTC_COUNT; ++i) {
        // Set configuration data
        config[i].ADCOPT = 1;
        config[i].DTEN = 1;
        config[i].REFON = 1;
        config[i].GPIO = 0b11111;
        config[i].VUV = 0x0FF;
        config[i].VOV = 0xF00;
        config[i].DCC = 0x0FF;
        config[i].DCTO = 0b1111;

        // Set status data
        status[i].SC = 0xFF00;
        status[i].ITMP = 0x00FF;
        status[i].VA = 0xFF00;
        status[i].VD = 0x00FF;
        status[i].COV = 0xF00;
        status[i].CUV = 0x0FF;
        status[i].REV = 0b1111;
        status[i].RSVD = 0b11;
        status[i].MUXFAIL = 1;
        status[i].THSD = 1;

        // Set sctrl data
        for (size_t j = 0; j < LTC6811_REG_BYTE_COUNT; ++j) {
            size_t index = i * LTC6811_SCTL_COUNT + j * 2;
            sctrl[index] = 0x0;
            sctrl[index + 1] = 0xF;
        }

        // Set comm data
        comm[i].icom0 = LTC6811_I2C_WICOM_START;
        comm[i].icom1 = LTC6811_I2C_WICOM_BLANK;
        comm[i].icom2 = LTC6811_I2C_WICOM_BLANK;
        comm[i].fcom0 = LTC6811_I2C_WFCOM_ACK;
        comm[i].fcom1 = LTC6811_I2C_WFCOM_ACK;
        comm[i].fcom2 = LTC6811_I2C_WFCOM_STOP;
        for (size_t j = 0; j < 3; ++j)
            comm[i].data[j] = 0x0F;

        // Convert configuration data to encoded bytes (for easier testing)
        config_bytes[i][0] = (config[i].GPIO << 3) | (config[i].REFON << 2) | (config[i].DTEN << 1) | (config[i].ADCOPT);
        config_bytes[i][1] = config[i].VUV;
        config_bytes[i][2] = (config[i].VOV << 4) | (config[i].VUV >> 8);
        config_bytes[i][3] = config[i].VOV >> 4;
        config_bytes[i][4] = config[i].DCC;
        config_bytes[i][5] = (config[i].DCTO << 4) | (config[i].DCC >> 8);

        // Convert S pin control data to encoded bytes (for easier testing)
        size_t sctrl_off = i * LTC6811_SCTL_COUNT;
        sctrl_bytes[i][0] = (sctrl[sctrl_off] & 0x0F) | ((sctrl[sctrl_off + 1] & 0x0F) << 4);
        sctrl_bytes[i][1] = (sctrl[sctrl_off + 2] & 0x0F) | ((sctrl[sctrl_off + 3] & 0x0F) << 4);
        sctrl_bytes[i][2] = (sctrl[sctrl_off + 4] & 0x0F) | ((sctrl[sctrl_off + 5] & 0x0F) << 4);
        sctrl_bytes[i][3] = (sctrl[sctrl_off + 6] & 0x0F) | ((sctrl[sctrl_off + 7] & 0x0F) << 4);
        sctrl_bytes[i][4] = (sctrl[sctrl_off + 8] & 0x0F) | ((sctrl[sctrl_off + 9] & 0x0F) << 4);
        sctrl_bytes[i][5] = (sctrl[sctrl_off + 10] & 0x0F) | ((sctrl[sctrl_off + 11] & 0x0F) << 4);

        // Convert comm data to encoded bytes (for easier testing)
        comm_bytes[i][0] = (comm[i].icom0 << 4) | (comm[i].data[0] >> 4);
        comm_bytes[i][1] = ((comm[i].data[0] & 0x0F) << 4) | comm[i].fcom0;
        comm_bytes[i][2] = (comm[i].icom1 << 4) | (comm[i].data[1] >> 4);
        comm_bytes[i][3] = ((comm[i].data[1] & 0x0F) << 4) | comm[i].fcom1;
        comm_bytes[i][4] = (comm[i].icom2 << 4) | (comm[i].data[2] >> 4);
        comm_bytes[i][5] = ((comm[i].data[2] & 0x0F) << 4) | comm[i].fcom2;
    }

    config_pec_bytes[0][0] = 0xE3;
    config_pec_bytes[0][1] = 0x6C;
    config_pec_bytes[1][0] = 0xE3;
    config_pec_bytes[1][1] = 0x6C;
    config_pec_bytes[2][0] = 0xE3;
    config_pec_bytes[2][1] = 0x6C;

    sctrl_pec_bytes[0][0] = 0x47; 
    sctrl_pec_bytes[0][1] = 0xC4; 
    sctrl_pec_bytes[1][0] = 0x47; 
    sctrl_pec_bytes[1][1] = 0xC4; 
    sctrl_pec_bytes[2][0] = 0x47; 
    sctrl_pec_bytes[2][1] = 0xC4; 

    comm_pec_bytes[0][0] = 0x3D;
    comm_pec_bytes[0][1] = 0x3A;
    comm_pec_bytes[1][0] = 0x3D;
    comm_pec_bytes[1][1] = 0x3A;
    comm_pec_bytes[2][0] = 0x3D;
    comm_pec_bytes[2][1] = 0x3A;
}

void tearDown(void) {

}

void check_wrcfg_encode_broadcast_length() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_wrcfg_encode_broadcast(
        &chain,
        config,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_wrcfg_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { WRCFGA >> 8, WRCFGA & 0xFF };
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrcfg_encode_broadcast(
        &chain,
        config,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_wrcfg_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[WRCFGA] >> 8, cmd_pecs[WRCFGA] & 0xFF };
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrcfg_encode_broadcast(
        &chain,
        config,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}
void check_wrcfg_encode_broadcast_data_bytes() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrcfg_encode_broadcast(
        &chain,
        config,
        out
    );
    size_t off = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < LTC_COUNT; ++i) {
        TEST_ASSERT_EQUAL_HEX8_ARRAY(config_bytes[LTC_COUNT - i - 1], out + off, LTC6811_REG_BYTE_COUNT);
        off += LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    }
}
void check_wrcfg_encode_broadcast_data_pec_bytes() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrcfg_encode_broadcast(
        &chain,
        config,
        out
    );
    size_t off = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < LTC_COUNT; ++i) {
        off += LTC6811_REG_BYTE_COUNT;
        TEST_ASSERT_EQUAL_HEX8_ARRAY(config_pec_bytes[LTC_COUNT - i - 1], out + off, LTC6811_PEC_BYTE_COUNT);
        off += LTC6811_PEC_BYTE_COUNT;
    }
}

void check_rdcfg_encode_broadcast_length() {
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_rdcfg_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_READ_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_rdcfg_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { RDCFGA >> 8, RDCFGA & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdcfg_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_rdcfg_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[RDCFGA] >> 8, cmd_pecs[RDCFGA] & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdcfg_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_rdcfg_decode_broadcast_length() {
    // Data from the encoded config array
    uint8_t data[] = {
        0xff, 0xff, 0x00, 0xf0, 0xff, 0xf0, 0xe3, 0x6c,
        0xff, 0xff, 0x00, 0xf0, 0xff, 0xf0, 0xe3, 0x6c,
        0xff, 0xff, 0x00, 0xf0, 0xff, 0xf0, 0xe3, 0x6c
    };
    Ltc6811Cfgr decoded[LTC_COUNT] = { 0 };
    size_t byte_count = ltc6811_rdcfg_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_size_t(LTC_COUNT * (LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT), byte_count);
}
void check_rdcfg_decode_broadcast_data() {
    // Data from the encoded config array (PEC included)
    uint8_t data[] = {
        0xff, 0xff, 0x00, 0xf0, 0xff, 0xf0, 0xe3, 0x6c,
        0xff, 0xff, 0x00, 0xf0, 0xff, 0xf0, 0xe3, 0x6c,
        0xff, 0xff, 0x00, 0xf0, 0xff, 0xf0, 0xe3, 0x6c
    };
    Ltc6811Cfgr decoded[LTC_COUNT] = { 0 };
    ltc6811_rdcfg_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(config, decoded, sizeof(Ltc6811Cfgr), LTC_COUNT);
}

void check_rdcv_encode_broadcast_length() {
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_rdcv_encode_broadcast(
        &chain,
        LTC6811_CVAR,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_READ_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_rdcv_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { RDCVA >> 8, RDCVA & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdcv_encode_broadcast(
        &chain,
        LTC6811_CVAR,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_rdcv_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[RDCVA] >> 8, cmd_pecs[RDCVA] & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdcv_encode_broadcast(
        &chain,
        LTC6811_CVAR,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_rdcv_decode_broadcast_length() {
    // Data from the encoded config array
    uint8_t data[] = {
        0x16, 0x00, 0x00, 0x00, 0x02, 0x70, 0x09, 0x74,
        0x0e, 0x00, 0x00, 0x00, 0x01, 0x70, 0xa0, 0x84,
        0x06, 0x00, 0x00, 0x00, 0x00, 0x70, 0x41, 0x3a
    };
    uint16_t decoded[LTC_COUNT * LTC6811_REG_CELL_COUNT] = { 0 };
    size_t byte_count = ltc6811_rdcv_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_size_t(LTC_COUNT * (LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT), byte_count);
}
void check_rdcv_decode_broadcast_data() {
    // Original cells voltages data
    uint16_t volts[] = {
        6, 0, 28672,
        14, 0, 28673,
        22, 0, 28674
    };
    // Data from the encoded config array (PEC included)
    uint8_t data[] = {
        0x06, 0x00, 0x00, 0x00, 0x00, 0x70, 0x41, 0x3a,
        0x0e, 0x00, 0x00, 0x00, 0x01, 0x70, 0xa0, 0x84,
        0x16, 0x00, 0x00, 0x00, 0x02, 0x70, 0x09, 0x74
    };
    uint16_t decoded[LTC_COUNT * LTC6811_REG_CELL_COUNT] = { 0 };
    ltc6811_rdcv_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_UINT16_ARRAY(volts, decoded, LTC_COUNT * LTC6811_REG_CELL_COUNT);
}

void check_rdaux_encode_broadcast_length() {
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_rdaux_encode_broadcast(
        &chain,
        LTC6811_AVAR,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_READ_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_rdaux_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { RDAUXA >> 8, RDAUXA & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdaux_encode_broadcast(
        &chain,
        LTC6811_AVAR,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_rdaux_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[RDAUXA] >> 8, cmd_pecs[RDAUXA] & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdaux_encode_broadcast(
        &chain,
        LTC6811_AVAR,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_rdaux_decode_broadcast_length() {
    // Data from the encoded config array
    uint8_t data[] = {
        0x16, 0x00, 0x00, 0x00, 0x02, 0x70, 0x09, 0x74,
        0x0e, 0x00, 0x00, 0x00, 0x01, 0x70, 0xa0, 0x84,
        0x06, 0x00, 0x00, 0x00, 0x00, 0x70, 0x41, 0x3a
    };
    uint16_t decoded[LTC_COUNT * LTC6811_REG_CELL_COUNT] = { 0 };
    size_t byte_count = ltc6811_rdaux_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_size_t(LTC_COUNT * (LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT), byte_count);
}
void check_rdaux_decode_broadcast_data() {
    // Original cells voltages data
    uint16_t volts[] = {
        6, 0, 28672,
        14, 0, 28673,
        22, 0, 28674
    };
    // Data from the encoded config array (PEC included)
    uint8_t data[] = {
        0x06, 0x00, 0x00, 0x00, 0x00, 0x70, 0x41, 0x3a,
        0x0e, 0x00, 0x00, 0x00, 0x01, 0x70, 0xa0, 0x84,
        0x16, 0x00, 0x00, 0x00, 0x02, 0x70, 0x09, 0x74
    };
    uint16_t decoded[LTC_COUNT * LTC6811_REG_CELL_COUNT] = { 0 };
    ltc6811_rdaux_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_UINT16_ARRAY(volts, decoded, LTC_COUNT * LTC6811_REG_CELL_COUNT);
}

void check_rdstat_encode_broadcast_length() {
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_rdstat_encode_broadcast(
        &chain,
        LTC6811_STAR,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_READ_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_rdstat_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { RDSTATA >> 8, RDSTATA & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdstat_encode_broadcast(
        &chain,
        LTC6811_STAR,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_rdstat_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[RDSTATA] >> 8, cmd_pecs[RDSTATA] & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdstat_encode_broadcast(
        &chain,
        LTC6811_STAR,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_rdstat_decode_broadcast_length() {
    // Data from the encoded status array (PEC included)
    uint8_t data[] = {
        0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x45, 0x4e,
        0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x45, 0x4e,
        0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x45, 0x4e
    };
    Ltc6811Str decoded[LTC_COUNT] = { 0 };
    size_t byte_count = ltc6811_rdstat_decode_broadcast(
        &chain,
        LTC6811_STAR,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_size_t(LTC_COUNT * (LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT), byte_count);
}
void check_rdstat_decode_broadcast_data() {
    // Data from the encoded status array (PEC included)
    uint8_t data_star[] = {
        0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x45, 0x4e,
        0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x45, 0x4e,
        0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x45, 0x4e
    };
    uint8_t data_stbr[] = {
        0xff, 0x00, 0x55, 0x55, 0xaa, 0xff, 0x44, 0xea,
        0xff, 0x00, 0x55, 0x55, 0xaa, 0xff, 0x44, 0xea,
        0xff, 0x00, 0x55, 0x55, 0xaa, 0xff, 0x44, 0xea
    };
    Ltc6811Str decoded[LTC_COUNT] = { 0 };
    ltc6811_rdstat_decode_broadcast(
        &chain,
        LTC6811_STAR,
        data_star,
        decoded
    );
    ltc6811_rdstat_decode_broadcast(
        &chain,
        LTC6811_STBR,
        data_stbr,
        decoded
    );
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(status, decoded, sizeof(Ltc6811Str), LTC_COUNT);
}

void check_wrsctrl_encode_broadcast_length() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_wrsctrl_encode_broadcast(
        &chain,
        sctrl,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_wrsctrl_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { WRSCTRL >> 8, WRSCTRL & 0xFF };
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrsctrl_encode_broadcast(
        &chain,
        sctrl,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_wrsctrl_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[WRSCTRL] >> 8, cmd_pecs[WRSCTRL] & 0xFF };
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrsctrl_encode_broadcast(
        &chain,
        sctrl,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}
void check_wrsctrl_encode_broadcast_data_bytes() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrsctrl_encode_broadcast(
        &chain,
        sctrl,
        out
    );
    size_t off = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < LTC_COUNT; ++i) {
        TEST_ASSERT_EQUAL_HEX8_ARRAY(sctrl_bytes[LTC_COUNT - i - 1], out + off, LTC6811_REG_BYTE_COUNT);
        off += LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    }
}
void check_wrsctrl_encode_broadcast_data_pec_bytes() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrsctrl_encode_broadcast(
        &chain,
        sctrl,
        out
    );
    size_t off = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < LTC_COUNT; ++i) {
        off += LTC6811_REG_BYTE_COUNT;
        TEST_ASSERT_EQUAL_HEX8_ARRAY(sctrl_pec_bytes[LTC_COUNT - i - 1], out + off, LTC6811_PEC_BYTE_COUNT);
        off += LTC6811_PEC_BYTE_COUNT;
    }
}

void check_rdsctrl_encode_broadcast_length() {
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_rdsctrl_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_READ_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_rdsctrl_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { RDSCTRL >> 8, RDSCTRL & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdsctrl_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_rdsctrl_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[RDSCTRL] >> 8, cmd_pecs[RDSCTRL] & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdsctrl_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_rdsctrl_decode_broadcast_length() {
    // Data from the encoded 'sctrl' array
    uint8_t data[] = {
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4,
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4,
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4
    };
    uint8_t decoded[LTC_COUNT * LTC6811_SCTL_COUNT] = { 0 };
    size_t byte_count = ltc6811_rdsctrl_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_size_t(LTC_COUNT * (LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT), byte_count);
}
void check_rdsctrl_decode_broadcast_data() {
    // Data from the encoded 'sctrl' array (PEC included)
    uint8_t data[] = {
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4,
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4,
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4
    };
    uint8_t decoded[LTC_COUNT * LTC6811_SCTL_COUNT] = { 0 };
    ltc6811_rdsctrl_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_UINT8_ARRAY(sctrl, decoded, LTC_COUNT * LTC6811_SCTL_COUNT);
}

void check_stsctrl_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_stsctrl_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_stsctrl_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { STSCTRL >> 8, STSCTRL & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_stsctrl_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_stsctrl_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[STSCTRL] >> 8, cmd_pecs[STSCTRL] & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_stsctrl_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_clrsctrl_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_clrsctrl_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_clrsctrl_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { CLRSCTRL >> 8, CLRSCTRL & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_clrsctrl_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_clrsctrl_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[CLRSCTRL] >> 8, cmd_pecs[CLRSCTRL] & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_clrsctrl_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

// The same data of the 'sctrl' command is used as 'pwm' data for simplicity
void check_wrpwm_encode_broadcast_length() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_wrpwm_encode_broadcast(
        &chain,
        sctrl,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_wrpwm_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { WRPWM >> 8, WRPWM & 0xFF };
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrpwm_encode_broadcast(
        &chain,
        sctrl,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_wrpwm_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[WRPWM] >> 8, cmd_pecs[WRPWM] & 0xFF };
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrpwm_encode_broadcast(
        &chain,
        sctrl,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}
void check_wrpwm_encode_broadcast_data_bytes() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrpwm_encode_broadcast(
        &chain,
        sctrl,
        out
    );
    size_t off = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < LTC_COUNT; ++i) {
        TEST_ASSERT_EQUAL_HEX8_ARRAY(sctrl_bytes[LTC_COUNT - i - 1], out + off, LTC6811_REG_BYTE_COUNT);
        off += LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    }
}
void check_wrpwm_encode_broadcast_data_pec_bytes() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrpwm_encode_broadcast(
        &chain,
        sctrl,
        out
    );
    size_t off = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < LTC_COUNT; ++i) {
        off += LTC6811_REG_BYTE_COUNT;
        TEST_ASSERT_EQUAL_HEX8_ARRAY(sctrl_pec_bytes[LTC_COUNT - i - 1], out + off, LTC6811_PEC_BYTE_COUNT);
        off += LTC6811_PEC_BYTE_COUNT;
    }
}

void check_rdpwm_encode_broadcast_length() {
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_rdpwm_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_READ_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_rdpwm_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { RDPWM >> 8, RDPWM & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdpwm_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_rdpwm_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[RDPWM] >> 8, cmd_pecs[RDPWM] & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdpwm_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_rdpwm_decode_broadcast_length() {
    // Data from the encoded 'sctrl' array
    uint8_t data[] = {
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4,
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4,
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4
    };
    uint8_t decoded[LTC_COUNT * LTC6811_PWM_COUNT] = { 0 };
    size_t byte_count = ltc6811_rdpwm_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_size_t(LTC_COUNT * (LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT), byte_count);
}
void check_rdpwm_decode_broadcast_data() {
    // Data from the encoded 'sctrl' array (PEC included)
    uint8_t data[] = {
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4,
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4,
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x47, 0xc4
    };
    uint8_t decoded[LTC_COUNT * LTC6811_PWM_COUNT] = { 0 };
    ltc6811_rdpwm_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_UINT8_ARRAY(sctrl, decoded, LTC_COUNT * LTC6811_PWM_COUNT);
}

void check_adcv_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_adcv_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        LTC6811_CH_12,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_adcv_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = ADCV | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_DCP_ENABLED << 4) | (LTC6811_CH_12);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adcv_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        LTC6811_CH_12,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_adcv_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0xc7, 0xac };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adcv_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        LTC6811_CH_12,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_adow_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_adow_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_PUP_ACTIVE,
        LTC6811_DCP_ENABLED,
        LTC6811_CH_12,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_adow_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = ADOW | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_PUP_ACTIVE << 6) | (LTC6811_DCP_ENABLED << 4) | (LTC6811_CH_12);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adow_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_PUP_ACTIVE,
        LTC6811_DCP_ENABLED,
        LTC6811_CH_12,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_adow_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0x2f, 0xa2 };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adow_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_PUP_ACTIVE,
        LTC6811_DCP_ENABLED,
        LTC6811_CH_12,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_cvst_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_cvst_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_ST_TWO,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_cvst_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = CVST | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_ST_TWO << 5);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_cvst_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_ST_TWO,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_cvst_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0xa1, 0xec };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_cvst_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_ST_TWO,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_adol_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_adol_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_adol_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = ADOL | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_DCP_ENABLED << 4);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adol_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_adol_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0x31, 0x80 };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adol_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_adax_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_adax_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHG_SECOND_REF,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_adax_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = ADAX | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_CHG_SECOND_REF);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adax_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHG_SECOND_REF,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_adax_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0xbb, 0x4e };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adax_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHG_SECOND_REF,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_adaxd_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_adaxd_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHG_SECOND_REF,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_adaxd_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = ADAXD | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_CHG_SECOND_REF);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adaxd_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHG_SECOND_REF,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_adaxd_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0xea, 0x98 };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adaxd_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHG_SECOND_REF,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_axst_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_axst_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_ST_TWO,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_axst_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = AXST | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_ST_TWO << 5);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_axst_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_ST_TWO,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_axst_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0x86, 0x20 };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_axst_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_ST_TWO,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_adstat_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_adstat_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHST_VD,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_adstat_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = ADSTAT | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_CHST_VD);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adstat_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHST_VD,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_adstat_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0xce, 0x16 };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adstat_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHST_VD,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_adstatd_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_adstatd_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHST_VD,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_adstatd_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = ADSTATD | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_CHST_VD);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adstatd_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHST_VD,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_adstatd_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0x9f, 0xc0 };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adstatd_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_CHST_VD,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_statst_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_statst_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_ST_TWO,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_statst_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = STATST | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_ST_TWO << 5);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_statst_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_ST_TWO,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_statst_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0x6e, 0x2e };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_statst_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_ST_TWO,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_adcvax_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_adcvax_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_adcvax_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = ADCVAX | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_DCP_ENABLED << 4);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adcvax_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_adcvax_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0x83, 0x5c };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adcvax_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_adcvsc_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_adcvsc_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_adcvsc_encode_broadcast_cmd_bytes() {
    Ltc6811Command command = ADCVSC | (LTC6811_MD_26HZ_2KHZ << 7) | (LTC6811_DCP_ENABLED << 4);
    uint8_t cmd[] = { command >> 8, command & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adcvsc_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_adcvsc_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { 0x6b, 0x52 };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_adcvsc_encode_broadcast(
        &chain,
        LTC6811_MD_26HZ_2KHZ,
        LTC6811_DCP_ENABLED,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_clrcell_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_clrcell_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_clrcell_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { CLRCELL >> 8, CLRCELL & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_clrcell_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_clrcell_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[CLRCELL] >> 8, cmd_pecs[CLRCELL] & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_clrcell_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_clraux_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_clraux_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_clraux_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { CLRAUX >> 8, CLRAUX & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_clraux_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_clraux_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[CLRAUX] >> 8, cmd_pecs[CLRAUX] & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_clraux_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_clrstat_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_clrstat_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_clrstat_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { CLRSTAT >> 8, CLRSTAT & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_clrstat_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_clrstat_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[CLRSTAT] >> 8, cmd_pecs[CLRSTAT] & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_clrstat_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_pladc_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_pladc_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_pladc_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { PLADC >> 8, PLADC & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_pladc_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_pladc_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[PLADC] >> 8, cmd_pecs[PLADC] & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_pladc_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_pladc_check_conversion_running() {
    TEST_ASSERT_FALSE(ltc6811_pladc_check(0x07));
}
void check_pladc_check_conversion_ended() {
    TEST_ASSERT_TRUE(ltc6811_pladc_check(0xFF));
}

void check_diagn_encode_broadcast_length() {
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_diagn_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_POLL_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_diagn_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { DIAGN >> 8, DIAGN & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_diagn_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_diagn_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[DIAGN] >> 8, cmd_pecs[DIAGN] & 0xFF };
    uint8_t out[LTC6811_POLL_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_diagn_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_wrcomm_encode_broadcast_length() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_wrcomm_encode_broadcast(
        &chain,
        comm,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_wrcomm_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { WRCOMM >> 8, WRCOMM & 0xFF };
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrcomm_encode_broadcast(
        &chain,
        comm,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_wrcomm_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[WRCOMM] >> 8, cmd_pecs[WRCOMM] & 0xFF };
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrcomm_encode_broadcast(
        &chain,
        comm,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}
void check_wrcomm_encode_broadcast_data_bytes() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrcomm_encode_broadcast(
        &chain,
        comm,
        out
    );
    size_t off = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < LTC_COUNT; ++i) {
        TEST_ASSERT_EQUAL_HEX8_ARRAY(comm_bytes[LTC_COUNT - i - 1], out + off, LTC6811_REG_BYTE_COUNT);
        off += LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    }
}
void check_wrcomm_encode_broadcast_data_pec_bytes() {
    uint8_t out[LTC6811_WRITE_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_wrcomm_encode_broadcast(
        &chain,
        comm,
        out
    );
    size_t off = LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT;
    for (size_t i = 0; i < LTC_COUNT; ++i) {
        off += LTC6811_REG_BYTE_COUNT;
        TEST_ASSERT_EQUAL_HEX8_ARRAY(comm_pec_bytes[LTC_COUNT - i - 1], out + off, LTC6811_PEC_BYTE_COUNT);
        off += LTC6811_PEC_BYTE_COUNT;
    }
}

void check_rdcomm_encode_broadcast_length() {
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_rdcomm_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_READ_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_rdcomm_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { RDCOMM >> 8, RDCOMM & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdcomm_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_rdcomm_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[RDCOMM] >> 8, cmd_pecs[RDCOMM] & 0xFF };
    uint8_t out[LTC6811_READ_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_rdcomm_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}

void check_rdcomm_decode_broadcast_length() {
    // Data from the encoded config array
    uint8_t data[] = {
        0x60, 0xf0, 0x00, 0xf0, 0x00, 0xf9, 0x3d, 0x3a,
        0x60, 0xf0, 0x00, 0xf0, 0x00, 0xf9, 0x3d, 0x3a,
        0x60, 0xf0, 0x00, 0xf0, 0x00, 0xf9, 0x3d, 0x3a
    };
    Ltc6811Comm decoded[LTC_COUNT] = { 0 };
    size_t byte_count = ltc6811_rdcomm_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_size_t(LTC_COUNT * (LTC6811_REG_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT), byte_count);
}
void check_rdcomm_decode_broadcast_data() {
    // Data from the encoded config array (PEC included)
    uint8_t data[] = {
        0x60, 0xf0, 0x00, 0xf0, 0x00, 0xf9, 0x3d, 0x3a,
        0x60, 0xf0, 0x00, 0xf0, 0x00, 0xf9, 0x3d, 0x3a,
        0x60, 0xf0, 0x00, 0xf0, 0x00, 0xf9, 0x3d, 0x3a
    };
    Ltc6811Comm decoded[LTC_COUNT] = { 0 };
    ltc6811_rdcomm_decode_broadcast(
        &chain,
        data,
        decoded
    );
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(comm, decoded, sizeof(Ltc6811Comm), LTC_COUNT);
}

void check_stcomm_encode_broadcast_length() {
    uint8_t out[LTC6811_STCOMM_BUFFER_SIZE(LTC_COUNT)];
    size_t byte_count = ltc6811_stcomm_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_size_t(LTC6811_STCOMM_BUFFER_SIZE(LTC_COUNT), byte_count);
}
void check_stcomm_encode_broadcast_cmd_bytes() {
    uint8_t cmd[] = { STCOMM >> 8, STCOMM & 0xFF };
    uint8_t out[LTC6811_STCOMM_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_stcomm_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmd, out, LTC6811_CMD_BYTE_COUNT);
}
void check_stcomm_encode_broadcast_cmd_pec_bytes() {
    uint8_t pec[] = { cmd_pecs[STCOMM] >> 8, cmd_pecs[STCOMM] & 0xFF };
    uint8_t out[LTC6811_STCOMM_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_stcomm_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pec, out + LTC6811_CMD_BYTE_COUNT, LTC6811_PEC_BYTE_COUNT);
}
void check_stcomm_encode_broadcast_dummy_bytes() {
    uint8_t out[LTC6811_STCOMM_BUFFER_SIZE(LTC_COUNT)];
    ltc6811_stcomm_encode_broadcast(
        &chain,
        out
    );
    TEST_ASSERT_EACH_EQUAL_HEX8(0xFF, out + LTC6811_CMD_BYTE_COUNT + LTC6811_PEC_BYTE_COUNT, LTC6811_STCOMM_CYCLES);
}


int main() {
    UNITY_BEGIN();

    // TODO: Make fixtures for each command

    // Write configuration broadcast encode
    RUN_TEST(check_wrcfg_encode_broadcast_length);
    RUN_TEST(check_wrcfg_encode_broadcast_cmd_bytes);
    RUN_TEST(check_wrcfg_encode_broadcast_cmd_pec_bytes);
    RUN_TEST(check_wrcfg_encode_broadcast_data_bytes);
    RUN_TEST(check_wrcfg_encode_broadcast_data_pec_bytes);

    // Read configuration broadcast encode
    RUN_TEST(check_rdcfg_encode_broadcast_length);
    RUN_TEST(check_rdcfg_encode_broadcast_cmd_bytes);
    RUN_TEST(check_rdcfg_encode_broadcast_cmd_pec_bytes);

    // Read configuration broadcast decode
    RUN_TEST(check_rdcfg_decode_broadcast_length);
    RUN_TEST(check_rdcfg_decode_broadcast_data);

    // Read cell voltages broadcast encode
    RUN_TEST(check_rdcv_encode_broadcast_length);
    RUN_TEST(check_rdcv_encode_broadcast_cmd_bytes);
    RUN_TEST(check_rdcv_encode_broadcast_cmd_pec_bytes);

    // Read cell voltages broadcast decode
    RUN_TEST(check_rdcv_decode_broadcast_length);
    RUN_TEST(check_rdcv_decode_broadcast_data);

    // Read auxiliary voltages broadcast encode
    RUN_TEST(check_rdaux_encode_broadcast_length);
    RUN_TEST(check_rdaux_encode_broadcast_cmd_bytes);
    RUN_TEST(check_rdaux_encode_broadcast_cmd_pec_bytes);

    // Read auxiliary voltages broadcast decode
    RUN_TEST(check_rdaux_decode_broadcast_length);
    RUN_TEST(check_rdaux_decode_broadcast_data);

    // Read status broadcast encode
    RUN_TEST(check_rdstat_encode_broadcast_length);
    RUN_TEST(check_rdstat_encode_broadcast_cmd_bytes);
    RUN_TEST(check_rdstat_encode_broadcast_cmd_pec_bytes);

    // Read status broadcast decode
    RUN_TEST(check_rdstat_decode_broadcast_length);
    RUN_TEST(check_rdstat_decode_broadcast_data);

    // Write S pin control data broadcast encode
    RUN_TEST(check_wrsctrl_encode_broadcast_length);
    RUN_TEST(check_wrsctrl_encode_broadcast_cmd_bytes);
    RUN_TEST(check_wrsctrl_encode_broadcast_cmd_pec_bytes);
    RUN_TEST(check_wrsctrl_encode_broadcast_data_bytes);
    RUN_TEST(check_wrsctrl_encode_broadcast_data_pec_bytes);

    // Read S pin control broadcast encode
    RUN_TEST(check_rdsctrl_encode_broadcast_length);
    RUN_TEST(check_rdsctrl_encode_broadcast_cmd_bytes);
    RUN_TEST(check_rdsctrl_encode_broadcast_cmd_pec_bytes);

    // Read S pin control broadcast decode
    RUN_TEST(check_rdsctrl_decode_broadcast_length);
    RUN_TEST(check_rdsctrl_decode_broadcast_data);

    // Start S pin control pulsing and poll status
    RUN_TEST(check_stsctrl_encode_broadcast_length);
    RUN_TEST(check_stsctrl_encode_broadcast_cmd_bytes);
    RUN_TEST(check_stsctrl_encode_broadcast_cmd_pec_bytes);

    // Start S pin control pulsing and poll status
    RUN_TEST(check_clrsctrl_encode_broadcast_length);
    RUN_TEST(check_clrsctrl_encode_broadcast_cmd_bytes);
    RUN_TEST(check_clrsctrl_encode_broadcast_cmd_pec_bytes);

    // Write PWM data broadcast encode
    RUN_TEST(check_wrpwm_encode_broadcast_length);
    RUN_TEST(check_wrpwm_encode_broadcast_cmd_bytes);
    RUN_TEST(check_wrpwm_encode_broadcast_cmd_pec_bytes);
    RUN_TEST(check_wrpwm_encode_broadcast_data_bytes);
    RUN_TEST(check_wrpwm_encode_broadcast_data_pec_bytes);

    // Read PWM data broadcast encode
    RUN_TEST(check_rdpwm_encode_broadcast_length);
    RUN_TEST(check_rdpwm_encode_broadcast_cmd_bytes);
    RUN_TEST(check_rdpwm_encode_broadcast_cmd_pec_bytes);

    // Read PWM data broadcast decode
    RUN_TEST(check_rdpwm_decode_broadcast_length);
    RUN_TEST(check_rdpwm_decode_broadcast_data);

    // Start cells ADC conversion and poll status broadcast encode
    RUN_TEST(check_adcv_encode_broadcast_length);
    RUN_TEST(check_adcv_encode_broadcast_cmd_bytes);
    RUN_TEST(check_adcv_encode_broadcast_cmd_pec_bytes);

    // Start open-wire ADC conversion and poll status broadcast encode
    RUN_TEST(check_adow_encode_broadcast_length);
    RUN_TEST(check_adow_encode_broadcast_cmd_bytes);
    RUN_TEST(check_adow_encode_broadcast_cmd_pec_bytes);

    // Start cell voltage ADC conversion self test and poll status broadcast encode
    RUN_TEST(check_cvst_encode_broadcast_length);
    RUN_TEST(check_cvst_encode_broadcast_cmd_bytes);
    RUN_TEST(check_cvst_encode_broadcast_cmd_pec_bytes);

    // Start cell 7 voltage overlap measurement broadcast encode
    RUN_TEST(check_adol_encode_broadcast_length);
    RUN_TEST(check_adol_encode_broadcast_cmd_bytes);
    RUN_TEST(check_adol_encode_broadcast_cmd_pec_bytes);

    // Start GPIOs ADC conversion an poll status broadcast encode
    RUN_TEST(check_adax_encode_broadcast_length);
    RUN_TEST(check_adax_encode_broadcast_cmd_bytes);
    RUN_TEST(check_adax_encode_broadcast_cmd_pec_bytes);

    // Start GPIOs ADC conversion with digital redundancy an poll status broadcast encode
    RUN_TEST(check_adaxd_encode_broadcast_length);
    RUN_TEST(check_adaxd_encode_broadcast_cmd_bytes);
    RUN_TEST(check_adaxd_encode_broadcast_cmd_pec_bytes);

    // Start GPIOs ADC conversion self test and poll status broadcast encode
    RUN_TEST(check_axst_encode_broadcast_length);
    RUN_TEST(check_axst_encode_broadcast_cmd_bytes);
    RUN_TEST(check_axst_encode_broadcast_cmd_pec_bytes);

    // Start status group ADC conversion an poll status broadcast encode
    RUN_TEST(check_adstat_encode_broadcast_length);
    RUN_TEST(check_adstat_encode_broadcast_cmd_bytes);
    RUN_TEST(check_adstat_encode_broadcast_cmd_pec_bytes);

    // Start status group ADC conversion an poll status broadcast encode
    RUN_TEST(check_adstatd_encode_broadcast_length);
    RUN_TEST(check_adstatd_encode_broadcast_cmd_bytes);
    RUN_TEST(check_adstatd_encode_broadcast_cmd_pec_bytes);

    // Start status group ADC conversion self test and poll status broadcast encode
    RUN_TEST(check_statst_encode_broadcast_length);
    RUN_TEST(check_statst_encode_broadcast_cmd_bytes);
    RUN_TEST(check_statst_encode_broadcast_cmd_pec_bytes);

    // Start cells voltage and GPIOs ADC conversion start and poll status broadcast encode
    RUN_TEST(check_adcvax_encode_broadcast_length);
    RUN_TEST(check_adcvax_encode_broadcast_cmd_bytes);
    RUN_TEST(check_adcvax_encode_broadcast_cmd_pec_bytes);

    // Start cells voltage and SC conversion start and poll status broadcast encode
    RUN_TEST(check_adcvsc_encode_broadcast_length);
    RUN_TEST(check_adcvsc_encode_broadcast_cmd_bytes);
    RUN_TEST(check_adcvsc_encode_broadcast_cmd_pec_bytes);

    // Clear cells voltage registers broadcast encode
    RUN_TEST(check_clrcell_encode_broadcast_length);
    RUN_TEST(check_clrcell_encode_broadcast_cmd_bytes);
    RUN_TEST(check_clrcell_encode_broadcast_cmd_pec_bytes);

    // Clear GPIOs voltage registers broadcast encode
    RUN_TEST(check_clraux_encode_broadcast_length);
    RUN_TEST(check_clraux_encode_broadcast_cmd_bytes);
    RUN_TEST(check_clraux_encode_broadcast_cmd_pec_bytes);

    // Clear status register broadcast encode
    RUN_TEST(check_clrstat_encode_broadcast_length);
    RUN_TEST(check_clrstat_encode_broadcast_cmd_bytes);
    RUN_TEST(check_clrstat_encode_broadcast_cmd_pec_bytes);

    // Poll for ADC conversion status broadcast encode
    RUN_TEST(check_pladc_encode_broadcast_length);
    RUN_TEST(check_pladc_encode_broadcast_cmd_bytes);
    RUN_TEST(check_pladc_encode_broadcast_cmd_pec_bytes);
    
    // Check ADC conversion status byte
    RUN_TEST(check_pladc_check_conversion_running);
    RUN_TEST(check_pladc_check_conversion_ended);

    // Diagnose MUX and poll status broadcast encode
    RUN_TEST(check_diagn_encode_broadcast_length);
    RUN_TEST(check_diagn_encode_broadcast_cmd_bytes);
    RUN_TEST(check_diagn_encode_broadcast_cmd_pec_bytes);

    // Write communication register broadcast encode
    RUN_TEST(check_wrcomm_encode_broadcast_length);
    RUN_TEST(check_wrcomm_encode_broadcast_cmd_bytes);
    RUN_TEST(check_wrcomm_encode_broadcast_cmd_pec_bytes);
    RUN_TEST(check_wrcomm_encode_broadcast_data_bytes);
    RUN_TEST(check_wrcomm_encode_broadcast_data_pec_bytes);

    // Read communication register broadcast encode
    RUN_TEST(check_rdcomm_encode_broadcast_length);
    RUN_TEST(check_rdcomm_encode_broadcast_cmd_bytes);
    RUN_TEST(check_rdcomm_encode_broadcast_cmd_pec_bytes);

    // Read communication register broadcast decode
    RUN_TEST(check_rdcomm_decode_broadcast_length);
    RUN_TEST(check_rdcomm_decode_broadcast_data);

    // Start external communication broadcast encode
    RUN_TEST(check_stcomm_encode_broadcast_length);
    RUN_TEST(check_stcomm_encode_broadcast_cmd_bytes);
    RUN_TEST(check_stcomm_encode_broadcast_cmd_pec_bytes);
    RUN_TEST(check_stcomm_encode_broadcast_dummy_bytes);

    UNITY_END();
}

