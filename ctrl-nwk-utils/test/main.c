#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../munit/munit.h"
#include "tests.h"

/* Define the test cases */
//TODO test for maximum space used for insert (-> PQ_insert() = false)
MunitTest tests[] = {
    {
        "/test-stuff-buffer",   /* name         */
        test_stuff_buffer,      /* test         */
        NULL,                   /* setup        */
        NULL,                   /* tear_down    */
        MUNIT_TEST_OPTION_NONE, /* options      */
        NULL                    /* parameters   */
    },
    {"/test-unstuff-buffer", test_unstuff_buffer, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test-compose-frame",  test_compose_frame,  NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

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
