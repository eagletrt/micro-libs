#ifndef TESTS_H
#define TESTS_H

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../munit/munit.h"

MunitResult test_is_empty(const MunitParameter *params, void *);
MunitResult test_ordered_insert(const MunitParameter *, void *);
MunitResult test_reverse_insert(const MunitParameter *, void *);
MunitResult test_random_insert(const MunitParameter *, void *);
MunitResult test_no_starvation(const MunitParameter *, void *);
MunitResult test_null_cmp_fn(const MunitParameter *, void *);
MunitResult test_pop_out_param(const MunitParameter *, void *);

#endif