/**
 * @file test-can-manager.c
 * @brief
 *
 * @date 15 Feb 2024
 * @author Giacomo Mazzucchi [giacomo.mazzucchi@protonmail.com]
 */

#include "can_manager.h"
#include "unity.h"

int can_manager_from_id_to_index(int can_id, int msg_id) { return 1; }

void setUp(void) {}

void tearDown(void) {}

void test_unity(void) { TEST_ASSERT(1); }

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_unity);

  UNITY_END();
}
