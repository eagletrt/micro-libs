#ifndef TEST_LIST_H
#define TEST_LIST_H

#include "munit.h"

void* test_list_setup(const MunitParameter params[], void* user_data);
MunitResult test_list_insert(const MunitParameter params[], void* user_data_or_fixture);

#endif