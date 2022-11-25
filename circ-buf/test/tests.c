#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../munit/munit.h"
#include "../circ-buf.h"
#include "tests.h"



MunitResult test_is_empty(const MunitParameter *params, void *data) {
    CBUF_HandleTypeDef cb;
    CBUF_init(&cb);

    assert_true(CBUF_is_empty(&cb));

    return MUNIT_OK;
}

MunitResult test_is_full(const MunitParameter *params, void *data) {
    CBUF_HandleTypeDef cb;
    CBUF_init(&cb);

    for (uint16_t i = 0; i < CBUF_SIZE - 1; i++)
        CBUF_enqueue(&cb, 0x00);

    assert_true(CBUF_is_full(&cb));
    
    return MUNIT_OK;
}

MunitResult test_order(const MunitParameter *params, void *data) {
    CBUF_HandleTypeDef cb;
    CBUF_init(&cb);

    for (uint16_t i = 0; i < CBUF_SIZE - 1; i++)
        CBUF_enqueue(&cb, i);

    for (uint16_t i = 0; i < CBUF_SIZE - 1; i++)
        assert_uint8(CBUF_dequeue(&cb), ==, i);
    
    return MUNIT_OK;
}
