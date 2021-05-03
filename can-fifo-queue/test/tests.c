#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../munit/munit.h"
#include "tests.h"

#include "../can_fifo_queue.h"


MunitResult test_is_empty(const MunitParameter * params, void * data) {
    CANFQ_QueueTypeDef q;
    CANFQ_init(&q);

    assert_true(CANFQ_is_empty(q));

    CAN_MessageTypeDef m = {0, 3, {1, 2, 3}};
    CANFQ_push(q, &m);

    assert_false(CANFQ_is_empty(q));

    CANFQ_destroy(&q);
    return MUNIT_OK;
}

MunitResult test_is_full(const MunitParameter * params, void * data) {
    CANFQ_QueueTypeDef q;
    CANFQ_init(&q);

    assert_false(CANFQ_is_full(q));

    for (int i = 0; i < CANFQ_SIZE - 1; i++) {
        CAN_MessageTypeDef m = {0, 3, {1, 2, 3}};
        bool status = CANFQ_push(q, &m);
        assert_true(status);
    }

    assert_true(CANFQ_is_full(q));

    CANFQ_destroy(&q);
    return MUNIT_OK;
}


MunitResult test_fifo_order(const MunitParameter * params, void * data) {
    CANFQ_QueueTypeDef q;
    CANFQ_init(&q);

    for (int i = 0; i < CANFQ_SIZE - 1; i++) {
        CAN_MessageTypeDef m = {i, 3, {1, 2, 3}};
        CANFQ_push(q, &m);
    }

    for (int i = 0; i < CANFQ_SIZE - 1; i++) {
        CAN_MessageTypeDef m;
        CANFQ_pop(q, &m);
        assert_uint16(m.id, ==, i);
    }

    CANFQ_destroy(&q);
    return MUNIT_OK;
}