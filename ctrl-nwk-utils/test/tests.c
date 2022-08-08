#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../munit/munit.h"
#include "../ctrl-nwk-utils.h"
#include "tests.h"


const uint8_t DLE = CTRL_DLE;
const uint8_t STX = CTRL_STX;
const uint8_t ETX = CTRL_ETX;


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

MunitResult test_unstuff_buffer_malformed(const MunitParameter *params, void *data) {
    uint8_t buf_in[10] = { 0x20, DLE, DLE, STX, 0x18, 0x07, DLE, DLE, DLE };
    uint8_t buf_out[7];
    uint8_t buf_expected[7] = { 0x20, DLE, STX, 0x18, 0x07, DLE };

    CTRL_unstuff_buffer(buf_in, 10, buf_out);

    assert_memory_equal(sizeof(buf_expected), buf_out, buf_expected);
    return MUNIT_OK;
}

MunitResult test_frame_wellformed_ok(const MunitParameter *paramas, void *data) {
    uint8_t frame[8] = {DLE, STX, 0x0, 0x0, 0x0, 0x0, DLE, ETX};
    assert_true(CTRL_is_frame_wellformed(frame, 8));
    return MUNIT_OK;
}

MunitResult test_frame_wellformed_ok_empty(const MunitParameter *paramas, void *data) {
    uint8_t frame[8] = {DLE, STX, DLE, ETX};
    assert_true(CTRL_is_frame_wellformed(frame, 4));
    return MUNIT_OK;
}

MunitResult test_frame_wellformed_bad(const MunitParameter *paramas, void *data) {
    uint8_t frame[8] = {DLE, DLE, STX, DLE, ETX, DLE};
    assert_false(CTRL_is_frame_wellformed(frame, 4));
    return MUNIT_OK;
}

MunitResult test_compose_frame(const MunitParameter *params, void *data) {
    CTRL_PayloadTypeDef payload;
    payload.ParamID = 0x00;
    payload.ParamVal = 5.42f;
    payload.CRC16 = 0x6655;

    uint8_t stuffed_buffer[11] = {};
    CTRL_compose_frame(&payload, stuffed_buffer);

    assert_uint8(stuffed_buffer[0],  ==, 0x10); // DLE
    assert_uint8(stuffed_buffer[1],  ==, 0x02); // STX
    assert_uint8(stuffed_buffer[2],  ==, 0x00); // ParamID
    assert_uint8(stuffed_buffer[3],  ==, 0xA4); // Float[0] - LSB
    assert_uint8(stuffed_buffer[4],  ==, 0x70); // Float[1]
    assert_uint8(stuffed_buffer[5],  ==, 0xAD); // Float[2]
    assert_uint8(stuffed_buffer[6],  ==, 0x40); // Float[3] - MSB
    assert_uint8(stuffed_buffer[7],  ==, 0x55); // CRC16[0] - LSB
    assert_uint8(stuffed_buffer[8],  ==, 0x66); // CRC16[1] - MSB
    assert_uint8(stuffed_buffer[9],  ==, 0x10); // DLE
    assert_uint8(stuffed_buffer[10], ==, 0x03); // ETX

    return MUNIT_OK;
}

MunitResult test_read_frame(const MunitParameter *params, void *data) {
    uint8_t stuffed_buffer[11] = {0x10, 0x02, 0x00, 0xA4, 0x70, 0xAD, 0x40, 0x55, 0x66, 0x10, 0x03};
    
    CTRL_PayloadTypeDef payload;
    CTRL_read_frame(stuffed_buffer, 11, &payload);    

    assert_uint8(payload.ParamID, ==, 0x00);
    assert_float(payload.ParamVal, ==, 5.42f);
    assert_uint16(payload.CRC16, ==, 0x6655);

    return MUNIT_OK;
}
