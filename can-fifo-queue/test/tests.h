#ifndef TESTS_H
#define TESTS_H

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../munit/munit.h"

MunitResult test_is_empty(const MunitParameter *, void *);
MunitResult test_is_full(const MunitParameter *, void *);
MunitResult test_fifo_order(const MunitParameter *, void *);

#endif