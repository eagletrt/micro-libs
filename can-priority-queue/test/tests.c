#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"
#include "tests.h"

#include "can.h"
#include "../can_priority_queue.h"

int _cmp_int(const void * a, const void * b) {
    return *(uint16_t*)a - *(uint16_t*)b;
}

MunitResult test_is_empty(const MunitParameter * params, void * data) {
    CANPQ_QueueTypeDef q;
    CANPQ_init(&q);

    assert_true(CANPQ_is_empty(q));

    CAN_MessageTypeDef m;
    CANPQ_insert(q, &m);

    assert_false(CANPQ_is_empty(q));

    CANPQ_destroy(&q);
    return MUNIT_OK;
}

MunitResult test_ordered_insert(const MunitParameter * params, void * data) {
    CANPQ_QueueTypeDef q;
    CANPQ_init(&q);

    for (int i = 0; i < CANPQ_SIZE; i++) {
        CAN_MessageTypeDef m = {i, i};
        CANPQ_insert(q, &m);
    }

    for (int i = 0; !CANPQ_is_empty(q); i++) {
        CAN_MessageTypeDef * m = CANPQ_peek_highest(q);
        assert_uint16(i, ==, m->data);
        CANPQ_pop_highest(q);
    }

    CANPQ_destroy(&q);
    return MUNIT_OK;
}

MunitResult test_reverse_insert(const MunitParameter * params, void * data) {
    CANPQ_QueueTypeDef q;
    CANPQ_init(&q);

    for (int i = CANPQ_SIZE-1; i >= 0; i--) {
        CAN_MessageTypeDef m = {i, i};
        CANPQ_insert(q, &m);
    }

    for (int i = 0; !CANPQ_is_empty(q); i++) {
        CAN_MessageTypeDef * m = CANPQ_peek_highest(q);
        assert_uint16(i, ==, m->data);
        CANPQ_pop_highest(q);
    }
    
    CANPQ_destroy(&q);    
    return MUNIT_OK;
}

MunitResult test_random_insert(const MunitParameter * params, void * data) {
    uint16_t IDs[CANPQ_SIZE];

    srand(42);
    for (int i = 0; i < CANPQ_SIZE; i++)
        IDs[i] = rand() % UINT16_MAX;

    CANPQ_QueueTypeDef q;
    CANPQ_init(&q);

    for (int i = 0; i < CANPQ_SIZE; i++) {
        CAN_MessageTypeDef m = {IDs[i], IDs[i]};
        CANPQ_insert(q, &m);
    }

    qsort(IDs, CANPQ_SIZE, sizeof(IDs[0]), _cmp_int);

    for (int i = 0; !CANPQ_is_empty(q); i++) {
        CAN_MessageTypeDef * m = CANPQ_peek_highest(q);
        assert_uint16(IDs[i], ==, m->data);
        CANPQ_pop_highest(q);
    }
    
    CANPQ_destroy(&q);    
    return MUNIT_OK;
}

MunitResult test_no_starvation(const MunitParameter * params, void * data) {
    CANPQ_QueueTypeDef q;
    CANPQ_init(&q);

    CAN_MessageTypeDef m1 = {50, (uint16_t)'A'};
    CANPQ_insert(q, &m1);

    for (int i = 0; i < CANPQ_SIZE; i++) {
        CAN_MessageTypeDef m_in = {1, (uint16_t)'B'};
        CANPQ_insert(q, &m_in);

        CAN_MessageTypeDef * m_out;
        m_out = CANPQ_peek_highest(q);

        /* After 99 pops, the first message will have reached equal priority (1) */
        assert_uint16(m_out->data, ==, (i == 49) ? 'A' : 'B');

        CANPQ_pop_highest(q);
    }

    CANPQ_destroy(&q);    
    return MUNIT_OK;
}