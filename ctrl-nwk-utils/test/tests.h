#ifndef TESTS_H
#define TESTS_H

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../munit/munit.h"

MunitResult test_stuff_buffer(const MunitParameter*, void *);
MunitResult test_unstuff_buffer(const MunitParameter*, void *);
MunitResult test_unstuff_buffer_malformed(const MunitParameter*, void *);
MunitResult test_frame_wellformed_ok(const MunitParameter*, void *);
MunitResult test_frame_wellformed_ok_empty(const MunitParameter*, void *);
MunitResult test_frame_wellformed_bad(const MunitParameter*, void *);
MunitResult test_compose_frame(const MunitParameter*, void *);
MunitResult test_read_frame(const MunitParameter*, void *);
MunitResult test_frame_CRC(const MunitParameter*, void *);

#endif
