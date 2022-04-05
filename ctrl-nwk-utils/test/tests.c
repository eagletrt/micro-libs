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