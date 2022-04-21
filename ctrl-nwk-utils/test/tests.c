#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../munit/munit.h"
#include "../ctrl-nwk-utils.h"
#include "tests.h"


const uint8_t DLE = 0x10;
const uint8_t STX = 0x02;
const uint8_t ETX = 0x03;


MunitResult test_stuff_buffer(const MunitParameter *params, void *data) {
    uint8_t buf_in[7] = { 0x20, DLE, STX, 0x18, 0x07, DLE, DLE };
    uint8_t buf_out[10];
    uint8_t buf_expected[10] = { 0x20, DLE, DLE, STX, 0x18, 0x07, DLE, DLE, DLE, DLE };

    CTRL_stuff_buffer(buf_in, 7, buf_out);

    assert_memory_equal(sizeof(buf_expected), buf_out, buf_expected);

    return MUNIT_OK;
}

MunitResult test_unstuff_buffer(const MunitParameter *params, void *data) {
    uint8_t buf_in[10] = { 0x20, DLE, DLE, STX, 0x18, 0x07, DLE, DLE, DLE, DLE };
    uint8_t buf_out[7];
    uint8_t buf_expected[7] = { 0x20, DLE, STX, 0x18, 0x07, DLE, DLE };

    CTRL_unstuff_buffer(buf_in, 10, buf_out);

    assert_memory_equal(sizeof(buf_expected), buf_out, buf_expected);

    return MUNIT_OK;
}

MunitResult test_compose_frame(const MunitParameter *params, void *data) {
    CTRL_PayloadTypeDef payload_1;
    payload_1.ParamID = 0x00;
    payload_1.ParamVal = 5.42f;
    payload_1.CRC = 0x6655;

    uint8_t stuffed_buffer[18] = {};
    uint8_t frame_len;
    frame_len = CTRL_compose_frame(&payload_1, stuffed_buffer);

    assert_uint8(stuffed_buffer[0],  ==, 0x10); // DLE
    assert_uint8(stuffed_buffer[1],  ==, 0x02); // STX
    assert_uint8(stuffed_buffer[2],  ==, 0x00); // ParamID
    assert_uint8(stuffed_buffer[3],  ==, 0xA4); // Float[0] - LSB
    assert_uint8(stuffed_buffer[4],  ==, 0x70); // Float[1]
    assert_uint8(stuffed_buffer[5],  ==, 0xAD); // Float[2]
    assert_uint8(stuffed_buffer[6],  ==, 0x40); // Float[3] - MSB
    assert_uint8(stuffed_buffer[7],  ==, 0x55); // CRC[0] - LSB
    assert_uint8(stuffed_buffer[8],  ==, 0x66); // CRC[1] - MSB
    assert_uint8(stuffed_buffer[9],  ==, 0x10); // DLE
    assert_uint8(stuffed_buffer[10], ==, 0x03); // ETX

    CTRL_PayloadTypeDef payload_2;
    CTRL_read_frame(stuffed_buffer, frame_len, &payload_2);    

    assert_uint8(payload_1.ParamID, ==, payload_2.ParamID);
    assert_float(payload_1.ParamVal, ==, payload_2.ParamVal);
    assert_uint16(payload_1.CRC, ==, payload_2.CRC);

    return MUNIT_OK;
}
