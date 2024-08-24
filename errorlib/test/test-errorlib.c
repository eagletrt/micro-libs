/**
 * @file test-errorlib.c
 * @brief Test for the critical error management library
 *
 * @date 23 Aug 2024
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#include "unity.h"
#include "errorlib.h"

#include <stddef.h>
#include <string.h>

#define ERROR_FIRST_INSTANCE_COUNT (1U)
#define ERROR_SECOND_INSTANCE_COUNT (34U)
#define ERROR_THIRD_INSTANCE_COUNT (23U)
#define ERROR_FOURTH_INSTANCE_COUNT (104U)
#define ERROR_FIFTH_INSTANCE_COUNT (69U)
#define ERROR_SIXTH_INSTANCE_COUNT (111U)
#define ERROR_SEVENTH_INSTANCE_COUNT (2U)
#define ERROR_EIGHTH_INSTANCE_COUNT (7U)
#define ERROR_NINTH_INSTANCE_COUNT (42U)
#define ERROR_TENTH_INSTANCE_COUNT (13U)

typedef enum {
    ERROR_GROUP_FIRST,
    ERROR_GROUP_SECOND,
    ERROR_GROUP_THIRD,
    ERROR_GROUP_FOURTH,
    ERROR_GROUP_FIFTH,
    ERROR_GROUP_SIXTH,
    ERROR_GROUP_SEVENTH,
    ERROR_GROUP_EIGHTH,
    ERROR_GROUP_NINTH,
    ERROR_GROUP_TENTH,
    ERROR_GROUP_COUNT
} ErrorGroup;

ErrorLibHandler herr;

const size_t instances[] = {
    [ERROR_GROUP_FIRST] = ERROR_FIRST_INSTANCE_COUNT,
    [ERROR_GROUP_SECOND] = ERROR_SECOND_INSTANCE_COUNT,
    [ERROR_GROUP_THIRD] = ERROR_THIRD_INSTANCE_COUNT,
    [ERROR_GROUP_FOURTH] = ERROR_FOURTH_INSTANCE_COUNT,
    [ERROR_GROUP_FIFTH] = ERROR_FIFTH_INSTANCE_COUNT,
    [ERROR_GROUP_SIXTH] = ERROR_SIXTH_INSTANCE_COUNT,
    [ERROR_GROUP_SEVENTH] = ERROR_SEVENTH_INSTANCE_COUNT,
    [ERROR_GROUP_EIGHTH] = ERROR_EIGHTH_INSTANCE_COUNT,
    [ERROR_GROUP_NINTH] = ERROR_NINTH_INSTANCE_COUNT,
    [ERROR_GROUP_TENTH] = ERROR_TENTH_INSTANCE_COUNT
};
const size_t thresholds[] = {
    [ERROR_GROUP_FIRST] = 1U,
    [ERROR_GROUP_SECOND] = 5U,
    [ERROR_GROUP_THIRD] = 10U,
    [ERROR_GROUP_FOURTH] = 20U,
    [ERROR_GROUP_FIFTH] = 2U,
    [ERROR_GROUP_SIXTH] = 3U,
    [ERROR_GROUP_SEVENTH] = 15U,
    [ERROR_GROUP_EIGHTH] = 8U,
    [ERROR_GROUP_NINTH] = 2U,
    [ERROR_GROUP_TENTH] = 10U
};
int32_t error_group_first[ERROR_FIRST_INSTANCE_COUNT];
int32_t error_group_second[ERROR_SECOND_INSTANCE_COUNT];
int32_t error_group_third[ERROR_THIRD_INSTANCE_COUNT];
int32_t error_group_fourth[ERROR_FOURTH_INSTANCE_COUNT];
int32_t error_group_fifth[ERROR_FIFTH_INSTANCE_COUNT];
int32_t error_group_sixth[ERROR_SIXTH_INSTANCE_COUNT];
int32_t error_group_seventh[ERROR_SEVENTH_INSTANCE_COUNT];
int32_t error_group_eighth[ERROR_EIGHTH_INSTANCE_COUNT];
int32_t error_group_ninth[ERROR_NINTH_INSTANCE_COUNT];
int32_t error_group_tenth[ERROR_TENTH_INSTANCE_COUNT];
int32_t * errors[] = {
    [ERROR_GROUP_FIRST] = error_group_first,
    [ERROR_GROUP_SECOND] = error_group_second,
    [ERROR_GROUP_THIRD] = error_group_third,
    [ERROR_GROUP_FOURTH] = error_group_fourth,
    [ERROR_GROUP_FIFTH] = error_group_fifth,
    [ERROR_GROUP_SIXTH] = error_group_sixth,
    [ERROR_GROUP_SEVENTH] = error_group_seventh,
    [ERROR_GROUP_EIGHTH] = error_group_eighth,
    [ERROR_GROUP_NINTH] = error_group_ninth,
    [ERROR_GROUP_TENTH] = error_group_tenth
};

void setUp(void) {
    errorlib_init(
        &herr,
        errors,
        instances,
        thresholds,
        ERROR_GROUP_COUNT
    );
    for (size_t i = 0U; i < ERROR_GROUP_COUNT; ++i)
        memset(errors[i], 0U, instances[i] * sizeof(int32_t));
}

void tearDown(void) {

}

void test_errorlib_error_set_invalid_group(void) {
    ErrorLibReturnCode code = errorlib_error_set(&herr, ERROR_GROUP_COUNT, 0U);
    TEST_ASSERT_EQUAL_INT(ERRORLIB_INVALID_GROUP, code);
}

void test_errorlib_error_set_invalid_instance(void) {
    ErrorLibReturnCode code = errorlib_error_set(&herr, ERROR_GROUP_FIRST, instances[ERROR_GROUP_FIRST]);
    TEST_ASSERT_EQUAL_INT(ERRORLIB_INVALID_INSTANCE, code);
}

void test_errorlib_error_set_return_code(void) {
    ErrorLibReturnCode code = errorlib_error_set(&herr, ERROR_GROUP_FIRST, 0U);
    TEST_ASSERT_EQUAL_INT32(ERRORLIB_OK, code);
}

void test_errorlib_error_reset_invalid_group(void) {
    ErrorLibReturnCode code = errorlib_error_reset(&herr, ERROR_GROUP_COUNT, 0U);
    TEST_ASSERT_EQUAL_INT(ERRORLIB_INVALID_GROUP, code);
}

void test_errorlib_error_reset_invalid_instance(void) {
    ErrorLibReturnCode code = errorlib_error_reset(&herr, ERROR_GROUP_FIRST, instances[ERROR_GROUP_FIRST]);
    TEST_ASSERT_EQUAL_INT(ERRORLIB_INVALID_INSTANCE, code);
}

void test_errorlib_error_reset_return_code(void) {
    ErrorLibReturnCode code = errorlib_error_reset(&herr, ERROR_GROUP_FIRST, 0U);
    TEST_ASSERT_EQUAL_INT32(ERRORLIB_OK, code);
}

void test_errorlib_error_set_single_counter_value(void) {
    const size_t inst = 0U;
    errorlib_error_set(&herr, ERROR_GROUP_TENTH, inst);
    TEST_ASSERT_EQUAL_INT32(1, error_group_tenth[inst]);
}

void test_errorlib_error_reset_single_counter_value(void) {
    const size_t inst = 0U;
    errorlib_error_reset(&herr, ERROR_GROUP_TENTH, inst);
    TEST_ASSERT_EQUAL_INT32(0, error_group_tenth[inst]);
}

void test_errorlib_error_expired_single_counter_value(void) {
    const size_t inst = 0U;
    for (size_t i = 0U; i < thresholds[ERROR_GROUP_TENTH]; ++i)
        errorlib_error_set(&herr, ERROR_GROUP_TENTH, inst);
    TEST_ASSERT_EQUAL_size_t(1U, errorlib_get_expired(&herr));
}

void test_errorlib_error_set_multi_counter_value(void) {
    /*
     * After the first error is expired every other error is ignored
     * this can be used to easily find which error has expired
     */
    const size_t inst = 0U;
    for (size_t i = 0U; i < ERROR_GROUP_COUNT; ++i) {
        errorlib_error_set(&herr, i, inst);
        char msg[64] = { 0 };
        sprintf(msg, "Error number %lu not set correctly", i);
        TEST_ASSERT_EQUAL_INT32_MESSAGE((i == ERROR_GROUP_FIRST) ? -1 : 0, errors[i][inst], msg);
    }
}

void test_errorlib_error_reset_multi_counter_value(void) {
    const size_t inst = 0U;
    for (size_t i = 0U; i < ERROR_GROUP_COUNT; ++i) {
        errorlib_error_reset(&herr, i, inst);
        char msg[64] = { 0 };
        sprintf(msg, "Error number %lu not reset correctly", i);
        TEST_ASSERT_EQUAL_INT32_MESSAGE(0, errors[i][inst], msg);
    }
}

void test_errorlib_error_expired_multi_counter_value(void) {
    /*
     * After the first error is expired every other error is ignored
     * this can be used to easily find which error has expired
     */
    const size_t inst = 0U;
    for (size_t group = 0U; group < ERROR_GROUP_COUNT; ++group) {
        for (size_t i = 0U; i < thresholds[group]; ++i)
            errorlib_error_set(&herr, group, inst);
    }
    TEST_ASSERT_EQUAL_size_t(1U, errorlib_get_expired(&herr));
}

void test_errorlib_error_get_status_invalid_group(void) {
    ErrorLibStatus status = errorlib_error_get_status(&herr, ERROR_GROUP_COUNT, 0U);
    TEST_ASSERT_EQUAL_INT(ERRORLIB_STATUS_UNSET, status);
}

void test_errorlib_error_get_status_invalid_instance(void) {
    ErrorLibStatus status = errorlib_error_get_status(&herr, ERROR_GROUP_FIRST, instances[ERROR_GROUP_FIRST]);
    TEST_ASSERT_EQUAL_INT(ERRORLIB_STATUS_UNSET, status);
}

void test_errorlib_error_get_status_when_zero(void) {
    errorlib_error_reset(&herr, ERROR_GROUP_FIFTH, 0U);
    ErrorLibStatus status = errorlib_error_get_status(&herr, ERROR_GROUP_FIFTH, 0U);
    TEST_ASSERT_EQUAL_INT(ERRORLIB_STATUS_UNSET, status);
}

void test_errorlib_error_get_status_when_positive_and_less_than_threshold(void) {
    errorlib_error_set(&herr, ERROR_GROUP_FIFTH, 0U);
    ErrorLibStatus status = errorlib_error_get_status(&herr, ERROR_GROUP_FIFTH, 0U);
    TEST_ASSERT_EQUAL_INT(ERRORLIB_STATUS_RUNNING, status);
}

void test_errorlib_error_get_status_when_positive_and_greater_than_threshold(void) {
    /*
     * The error set function does not allow to reach a value greater than the threshold
     * so it has to be done manually
     */ 
    error_group_fifth[0U] = instances[ERROR_GROUP_FIFTH];
    ErrorLibStatus status = errorlib_error_get_status(&herr, ERROR_GROUP_FIFTH, 0U);
    TEST_ASSERT_EQUAL_INT(ERRORLIB_STATUS_EXPIRED, status);
}

void test_errorlib_error_get_status_when_negative(void) {
    for (size_t i = 0U; i < instances[ERROR_GROUP_FIFTH]; ++i)
        errorlib_error_set(&herr, ERROR_GROUP_FIFTH, 0U);
    ErrorLibStatus status = errorlib_error_get_status(&herr, ERROR_GROUP_FIFTH, 0U);
    TEST_ASSERT_EQUAL_INT(ERRORLIB_STATUS_EXPIRED, status);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_errorlib_error_set_invalid_group);
    RUN_TEST(test_errorlib_error_set_invalid_instance);
    RUN_TEST(test_errorlib_error_set_return_code);
    RUN_TEST(test_errorlib_error_reset_invalid_group);
    RUN_TEST(test_errorlib_error_reset_invalid_instance);
    RUN_TEST(test_errorlib_error_reset_return_code);

    RUN_TEST(test_errorlib_error_set_single_counter_value);
    RUN_TEST(test_errorlib_error_reset_single_counter_value);
    RUN_TEST(test_errorlib_error_expired_single_counter_value);

    RUN_TEST(test_errorlib_error_set_multi_counter_value);
    RUN_TEST(test_errorlib_error_reset_multi_counter_value);
    RUN_TEST(test_errorlib_error_expired_multi_counter_value);

    RUN_TEST(test_errorlib_error_get_status_invalid_group);
    RUN_TEST(test_errorlib_error_get_status_invalid_instance);
    RUN_TEST(test_errorlib_error_get_status_when_zero);
    RUN_TEST(test_errorlib_error_get_status_when_positive_and_less_than_threshold);
    RUN_TEST(test_errorlib_error_get_status_when_positive_and_greater_than_threshold);
    RUN_TEST(test_errorlib_error_get_status_when_negative);

    UNITY_END();
}

