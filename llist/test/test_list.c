#include "test_list.h"

#include <stdio.h>
#include <string.h>

#include "llist.h"

struct data {
	int integer;
	char string[255];
};

#define TEST_ARR_SIZE 8

struct data test_arr[TEST_ARR_SIZE] = {
	{3, "tre"},
	{-45, "re"},
	{78, "tretto"},
	{8, ""},
	{600000, "#####"},
	{0, "42a0orem ipsum lorem ipsum dlorem ipsum dlorem ipsum dlorem ipsum dlorem ipsum do"},
	{-5000, "lorem ipsum dolor sit amet"},
	{4, "tre"}};

bool data_equal(llist_node a, llist_node b) {
	return (((struct data*)a)->integer == ((struct data*)b)->integer && strcmp(((struct data*)a)->string, ((struct data*)b)->string) == 0);
}
int8_t data_compare(llist_node a, llist_node b) {
	if (((struct data*)a)->integer > ((struct data*)b)->integer) {
		return 1;
	} else if ((((struct data*)a)->integer == ((struct data*)b)->integer)) {
		return 0;
	}
	return -1;
}

void* setup_empty(const MunitParameter params[], void* user_data) {
	return llist_init(data_compare, data_equal);
}

void* setup(const MunitParameter params[], void* user_data) {
	llist list = llist_init(data_compare, data_equal);

	for (size_t i = 0; i < TEST_ARR_SIZE; i++) {
		struct data* tmp = malloc(sizeof(struct data));
		munit_assert_not_null(tmp);

		tmp->integer = test_arr[i].integer;
		strcpy(tmp->string, test_arr[i].string);
		munit_assert_uint32(llist_push_back(list, (llist_node)tmp), ==, LLIST_SUCCESS);
	}

	munit_assert_size(llist_size(list), ==, TEST_ARR_SIZE);

	return list;
}

void tear_down(void* fixture) {
	llist_destroy((llist)fixture);
}

MunitResult test_push(const MunitParameter params[], void* user_data_or_fixture) {
	llist list = (llist)user_data_or_fixture;

	struct data* tmp = malloc(sizeof(struct data));
	munit_assert_not_null(tmp);

	tmp->integer = test_arr[3].integer;
	strcpy(tmp->string, test_arr[3].string);

	// Insert
	munit_assert_uint8(llist_push(list, (llist_node)tmp), ==, LLIST_SUCCESS);
	munit_assert_size(llist_size(list), ==, 1);

	// check if Head == Tail == tmp
	munit_assert_ptr_equal(llist_get_head(list), llist_get_tail(list));
	munit_assert_ptr_equal((llist_node)tmp, llist_get_head(list));

	struct data* tmp2 = malloc(sizeof(struct data));

	tmp2->integer = test_arr[1].integer;
	strcpy(tmp2->string, test_arr[1].string);

	munit_assert_uint8(llist_push(list, (llist_node)tmp2), ==, LLIST_SUCCESS);
	munit_assert_size(llist_size(list), ==, 2);

	munit_assert_ptr_not_equal(llist_get_head(list), tmp);
	munit_assert_ptr_equal(llist_get_head(list), tmp2);

	return MUNIT_OK;
}

MunitResult test_push_back(const MunitParameter params[], void* user_data_or_fixture) {
	llist list = (llist)user_data_or_fixture;

	struct data* tmp = malloc(sizeof(struct data));
	munit_assert_not_null(tmp);

	tmp->integer = test_arr[3].integer;
	strcpy(tmp->string, test_arr[3].string);

	// Insert
	munit_assert_uint8(llist_push_back(list, (llist_node)tmp), ==, LLIST_SUCCESS);
	munit_assert_size(llist_size(list), ==, 1);

	// check if Head == Tail == tmp
	munit_assert_ptr_equal(llist_get_head(list), llist_get_tail(list));
	munit_assert_ptr_equal((llist_node)tmp, llist_get_head(list));

	struct data* tmp2 = malloc(sizeof(struct data));

	tmp2->integer = test_arr[1].integer;
	strcpy(tmp2->string, test_arr[1].string);

	munit_assert_uint8(llist_push_back(list, (llist_node)tmp2), ==, LLIST_SUCCESS);
	munit_assert_size(llist_size(list), ==, 2);

	munit_assert_ptr_not_equal(llist_get_tail(list), tmp);
	munit_assert_ptr_equal(llist_get_tail(list), tmp2);

	return MUNIT_OK;
}

MunitResult test_get(const MunitParameter params[], void* user_data_or_fixture) {
	llist_node tmp;
	munit_assert_uint8(llist_get((llist)user_data_or_fixture, 1, &tmp), ==, LLIST_SUCCESS);
	munit_assert_true(data_equal(tmp, (llist_node)&test_arr[1]));

	return MUNIT_OK;
}

MunitResult test_find(const MunitParameter params[], void* user_data_or_fixture) {
	llist list = (llist)user_data_or_fixture;

	size_t count = llist_size(list);
	size_t found_index;

	for (size_t it = 0; it < count; it++) {
		munit_assert_uint32(llist_find(list, (llist_node)(&test_arr[it]), &found_index), ==, LLIST_SUCCESS);
		munit_assert_size(found_index, ==, it);
	}

	munit_assert_uint32(llist_find(list, (llist_node)&test_arr[TEST_ARR_SIZE], &found_index), ==, LLIST_NODE_NOT_FOUND);
	return MUNIT_OK;
}

MunitResult test_remove_by_index(const MunitParameter params[], void* user_data_or_fixture) {
	llist list = (llist)user_data_or_fixture;
	size_t count = llist_size(list);

	// Remove all nodes in random order
	// TODO: Uncomment this
	while (count > 1) {
		munit_assert_uint32(llist_remove_by_index(list, (size_t)munit_rand_int_range(0, count - 1)), ==, LLIST_SUCCESS);
		munit_assert_size(llist_size(list), ==, --count);
	}

	// TODO: bug in munit when calling random(0, 0). Remove this when fixed upstream
	munit_assert_uint32(llist_remove_by_index(list, 0), ==, LLIST_SUCCESS);
	munit_assert_size(llist_size(list), ==, 0);
	return MUNIT_OK;
}

MunitResult test_remove_by_node(const MunitParameter params[], void* user_data_or_fixture) {
	llist list = (llist)user_data_or_fixture;
	size_t count = llist_size(list);

	// Remove all nodes in random order
	// TODO: Uncomment this
	while (count > 1) {
		llist_node to_remove = NULL;
		munit_assert_uint32(llist_get(list, munit_rand_int_range(0, count - 1), &to_remove), ==, LLIST_SUCCESS);  //(llist_node)&test_arr[munit_rand_int_range(0, count)];

		munit_assert_uint32(llist_remove_by_node(list, to_remove), ==, LLIST_SUCCESS);
		munit_assert_size(llist_size(list), ==, --count);
	}

	llist_node to_remove = llist_get_head(list);  //(llist_node)&test_arr[munit_rand_int_range(0, count)];

	munit_assert_uint32(llist_remove_by_node(list, to_remove), ==, LLIST_SUCCESS);
	munit_assert_size(llist_size(list), ==, --count);

	return MUNIT_OK;
}

MunitResult test_export(const MunitParameter params[], void* user_data_or_fixture) {
	llist list = (llist)user_data_or_fixture;
	size_t count = llist_size(list);

	struct data verify_data[count];

	//struct data* verify_data = (llist_node *) malloc(sizeof(struct data) * count);
	munit_assert_uint32(llist_export(list, (void*)verify_data, sizeof(struct data)), ==, LLIST_SUCCESS);
	for (size_t i = 0; i < count; i++) {
		munit_assert_int(data_equal((llist_node)&verify_data[i], (llist_node)&test_arr[i]), ==, 1);
	}

	return MUNIT_OK;
}

MunitResult test_insert_priority(const MunitParameter params[], void* user_data_or_fixture) {
	llist list = (llist)user_data_or_fixture;

	// Insert
	for (size_t i = 0; i < TEST_ARR_SIZE; i++) {
		struct data* tmp = malloc(sizeof(struct data));
		munit_assert_not_null(tmp);

		tmp->integer = test_arr[i].integer;
		strcpy(tmp->string, test_arr[i].string);

		munit_assert_uint32(llist_insert_priority(list, (llist_node)tmp), ==, LLIST_SUCCESS);
	}

	// Verify
	struct data verify_data[TEST_ARR_SIZE];
	munit_assert_uint32(llist_export(list, (void*)verify_data, sizeof(struct data)), ==, LLIST_SUCCESS);

	for (size_t i = 1; i < TEST_ARR_SIZE; i++) {
		munit_assert_int(data_compare((llist_node)&verify_data[i - 1], (llist_node)&verify_data[i]), >=, 0);
	}

	return MUNIT_OK;
}

MunitTest test_list_tests[] = {
	{(char*)"/push", test_push, setup_empty, tear_down, MUNIT_TEST_OPTION_NONE, NULL},
	{(char*)"/push_back", test_push_back, setup_empty, tear_down, MUNIT_TEST_OPTION_NONE, NULL},
	{(char*)"/get", test_get, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL},
	{(char*)"/export", test_export, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL},
	{(char*)"/insert_priority", test_insert_priority, setup_empty, tear_down, MUNIT_TEST_OPTION_NONE, NULL},
	{(char*)"/find", test_find, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL},
	{(char*)"/remove_by_index", test_remove_by_index, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL},
	{(char*)"/remove_by_node", test_remove_by_node, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL},

	{NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, MUNIT_SUITE_OPTION_NONE}};

MunitSuite test_list_suite = {
	"/list",
	test_list_tests,
	NULL,
	1,
	MUNIT_SUITE_OPTION_NONE};
