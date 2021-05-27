#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../munit/munit.h"
#include "../priority_queue.h"
#include "tests.h"

#define PQ_SIZE 100

typedef struct msg {
    uint8_t n;
    char c[8];
} msg;

int _cmp_int(const void *a, const void *b) {
    return *(uint16_t *)a - *(uint16_t *)b;
}

int16_t _cmp_op(PQ_PriorityTypeDef a, PQ_PriorityTypeDef b) {
    return b - a;
}

void _pop_op(PQ_PriorityTypeDef *p) {
    (*p)--;
}

MunitResult test_is_empty(const MunitParameter *params, void *data) {
    PQ_QueueTypeDef q;
    PQ_init(&q, PQ_SIZE, sizeof(msg), _cmp_op, _pop_op);

    assert_true(PQ_is_empty(q));

    msg m = {1, "abc"};
    PQ_insert(q, 1, &m);

    assert_false(PQ_is_empty(q));

    PQ_destroy(&q);
    return MUNIT_OK;
}

MunitResult test_ordered_insert(const MunitParameter *params, void *data) {
    PQ_QueueTypeDef q;

    PQ_init(&q, PQ_SIZE, sizeof(msg), _cmp_op, _pop_op);

    for (int i = 0; i < PQ_SIZE; i++) {
        msg m = {i, "abc"};
        PQ_insert(q, i, &m);
    }

    for (int i = 0; !PQ_is_empty(q); i++) {
        msg *m = (msg *)PQ_peek_highest(q);
        assert_uint16(i, ==, m->n);
        PQ_pop_highest(q, NULL);
    }

    PQ_destroy(&q);
    return MUNIT_OK;
}

MunitResult test_reverse_insert(const MunitParameter *params, void *data) {
    PQ_QueueTypeDef q;
    PQ_init(&q, PQ_SIZE, sizeof(msg), _cmp_op, _pop_op);

    for (int i = PQ_SIZE - 1; i >= 0; i--) {
        msg m = {i, "abc"};
        PQ_insert(q, i, &m);
    }

    for (int i = 0; !PQ_is_empty(q); i++) {
        msg *m = (msg *)PQ_peek_highest(q);
        assert_uint16(i, ==, m->n);
        PQ_pop_highest(q, NULL);
    }

    PQ_destroy(&q);
    return MUNIT_OK;
}

MunitResult test_random_insert(const MunitParameter *params, void *data) {
    PQ_PriorityTypeDef IDs[PQ_SIZE];

    srand(42);
    for (int i = 0; i < PQ_SIZE; i++)
        IDs[i] = rand() % UINT8_MAX;

    PQ_QueueTypeDef q;
    PQ_init(&q, PQ_SIZE, sizeof(msg), _cmp_op, _pop_op);

    for (int i = 0; i < PQ_SIZE; i++) {
        msg m = {IDs[i], "abc"};
        PQ_insert(q, IDs[i], &m);
    }

    qsort(IDs, PQ_SIZE, sizeof(IDs[0]), _cmp_int);

    for (int i = 0; !PQ_is_empty(q); i++) {
        msg *m = (msg *)PQ_peek_highest(q);
        assert_uint16(IDs[i], ==, m->n);
        PQ_pop_highest(q, NULL);
    }

    PQ_destroy(&q);
    return MUNIT_OK;
}

MunitResult test_no_starvation(const MunitParameter *params, void *data) {
    PQ_QueueTypeDef q;
    PQ_init(&q, PQ_SIZE, sizeof(msg), _cmp_op, _pop_op);

    msg m1 = {50, "A"};
    PQ_insert(q, 50, &m1);

    for (int i = 0; i < PQ_SIZE; i++) {
        msg m_in = {1, "B"};
        PQ_insert(q, 1, &m_in);

        msg *m_out;
        m_out = PQ_peek_highest(q);

        /* After 99 pops, the first message will have reached equal priority (1)
		 */
        assert_uint16(m_out->n, ==, (i == 49) ? 50 : 1);

        PQ_pop_highest(q, NULL);
    }

    PQ_destroy(&q);
    return MUNIT_OK;
}

MunitResult test_null_cmp_fn(const MunitParameter *params, void *data) {
    PQ_QueueTypeDef q;
    PQ_init(&q, PQ_SIZE, sizeof(msg), NULL, _pop_op);

    for (int i = 0; i < PQ_SIZE; i++) {
        msg m = {i, "abc"};
        PQ_insert(q, i, &m);
    }

    for (int i = 0; !PQ_is_empty(q); i++) {
        msg *m = (msg *)PQ_peek_highest(q);
        assert_uint16(i, ==, m->n);
        PQ_pop_highest(q, NULL);
    }

    PQ_destroy(&q);
    return MUNIT_OK;
}

MunitResult test_pop_out_param(const MunitParameter *params, void *data) {
    PQ_QueueTypeDef q;
    PQ_init(&q, PQ_SIZE, sizeof(msg), _cmp_op, _pop_op);

    for (int i = 0; i < PQ_SIZE; i++) {
        msg m = {i, "abc"};
        PQ_insert(q, i, &m);
    }

    for (int i = 0; !PQ_is_empty(q); i++) {
        msg m;
        PQ_pop_highest(q, &m);
        assert_uint16(i, ==, m.n);
    }

    PQ_destroy(&q);
    return MUNIT_OK;
}
