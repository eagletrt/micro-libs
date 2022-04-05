#ifndef TESTS_H
#define TESTS_H

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../munit/munit.h"

MunitResult test_stuff_buffer(const MunitParameter*, void *);
MunitResult test_unstuff_buffer(const MunitParameter*, void *);

#endif