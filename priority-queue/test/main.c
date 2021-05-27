#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../munit/munit.h"
#include "tests.h"

/* Define the test cases */
//TODO test for maximum space used for insert (-> PQ_insert() = false)
MunitTest tests[] = {
    {
        "/test-is-empty",       /* name         */
        test_is_empty,          /* test         */
        NULL,                   /* setup        */
        NULL,                   /* tear_down    */
        MUNIT_TEST_OPTION_NONE, /* options      */
        NULL                    /* parameters   */
    },
    {"/test-ordered-insert", test_ordered_insert, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test-reverse-insert", test_reverse_insert, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test-random-insert", test_random_insert, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test-no-starvation", test_no_starvation, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test-null-cmp-fn", test_null_cmp_fn, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test-pop-out-param", test_pop_out_param, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    /* Mark the end of the array with a NULL test function */
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

/* Create a test suite */
static const MunitSuite suite = {
    "/test-suite",          /* name         */
    tests,                  /* tests        */
    NULL,                   /* suites       */
    1,                      /* iterations   */
    MUNIT_SUITE_OPTION_NONE /* options      */
};

/* Invoke the test suite */
int main(int argc, const char **argv) {
    return munit_suite_main(&suite, NULL, argc, argv);
}