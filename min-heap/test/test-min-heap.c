/**
 * @file test-min-int_heap.c
 * @brief Unit test functions for the min heap library
 *
 * @date 29 Feb 2024
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#include "unity.h"
#include "min-heap.h"

typedef struct {
    float x, y;
} Point;

int8_t min_heap_compare_float(void * f, void * s) {
    float a = *(float *)f;
    float b = *(float *)s;
    if (a < b)
        return -1;
    return a == b ? 0 : 1;
}
int8_t min_heap_compare_int(void * f, void * s) {
    int a = *(int *)f;
    int b = *(int *)s;
    if (a < b)
        return -1;
    return a == b ? 0 : 1;
}
int8_t min_heap_compare_point(void * f, void * s) {
    Point * a = (Point *)f;
    Point * b = (Point *)s;
    size_t dist_a = a->x * a->x + a->y * a->y;
    size_t dist_b = b->x * b->x + b->y * b->y;
    if (dist_a < dist_b)
        return -1;
    return dist_a == dist_b ? 0 : 1;
}

MinHeap(int, 10) int_heap;
MinHeap(Point, 10) point_heap;

void setUp(void) {
    min_heap_init(&int_heap, int, 10, min_heap_compare_int);
    min_heap_init(&point_heap, Point, 10, min_heap_compare_point);
}

void tearDown(void) {
    min_heap_clear(&int_heap);
    min_heap_clear(&point_heap);
}

void check_min_heap_init_with_null_handler(void) {
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_NULL_POINTER, min_heap_init(NULL, float, 3, min_heap_compare_float));
}
void check_min_heap_init_with_null_callback(void) {
    MinHeap(float, 3) heap;
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_NULL_POINTER, min_heap_init(&heap, float, 3, NULL));
}
void check_min_heap_init(void) {
    MinHeap(float, 3) heap;
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_OK, min_heap_init(&heap, float, 3, min_heap_compare_float));
}

void check_min_heap_size_empty(void) {
    TEST_ASSERT_EQUAL_size_t(min_heap_size(&int_heap), 0U);
}
void check_min_heap_size_not_empty(void) {
    // The size depends exclusively on the size variable
    int_heap.size = 3;
    TEST_ASSERT_EQUAL_size_t(min_heap_size(&int_heap), 3U);
}

void check_min_heap_empty_with_null(void) {
    TEST_ASSERT_TRUE(min_heap_is_empty(NULL));
}
void check_min_heap_empty_when_empty(void) {
    TEST_ASSERT_TRUE(min_heap_is_empty(&int_heap));
}
void check_min_heap_empty_when_not_empty(void) {
    // The empty check depends exclusively on the size variable
    int_heap.size = 3;
    TEST_ASSERT_FALSE(min_heap_is_empty(&int_heap));
}

void check_min_heap_full_with_null(void) {
    TEST_ASSERT_TRUE(min_heap_is_full(NULL));
}
void check_min_heap_full_when_full(void) {
    // The full check depends exclusively on the size variable
    int_heap.size = int_heap.capacity;
    TEST_ASSERT_TRUE(min_heap_is_full(&int_heap));
}
void check_min_heap_full_when_not_full(void) {
    TEST_ASSERT_FALSE(min_heap_is_full(&int_heap));
}

void check_min_heap_top_with_null_heap(void) {
    int a;
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_NULL_POINTER, min_heap_top(NULL, &a));
}
void check_min_heap_top_with_null_item(void) {
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_NULL_POINTER, min_heap_top(&int_heap, NULL));
}
void check_min_heap_top_when_empty(void) {
    int a;
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_EMPTY, min_heap_top(&int_heap, &a));
}
void check_min_heap_top_when_not_empty_return_value(void) {
    const int val = 7;
    int_heap.size = 1;
    int_heap.data[0] = val;
    int a;
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_OK, min_heap_top(&int_heap, &a));
}
void check_min_heap_top_when_not_empty_data(void) {
    const int val = 7;
    int_heap.size = 1;
    int_heap.data[0] = val;
    int a = 0;
    min_heap_top(&int_heap, &a);
    TEST_ASSERT_EQUAL_INT(val, a);
}

void check_min_heap_peek_with_null(void) {
    TEST_ASSERT_NULL(min_heap_peek(NULL));
}
void check_min_heap_peek_when_empty(void) {
    TEST_ASSERT_NULL(min_heap_peek(&int_heap));
}
void check_min_heap_peek_when_not_empty(void) {
    const int val = 5;
    int_heap.size = 1;
    int_heap.data[0] = val;
    int * item = (int *)min_heap_peek(&int_heap);
    TEST_ASSERT_NOT_NULL_MESSAGE(item, "The pointer to the first item is NULL");
    TEST_ASSERT_NOT_EQUAL_HEX64_MESSAGE(item, &val, "The address of the item is the same as the input variable");
    TEST_ASSERT_EQUAL_INT_MESSAGE(*item, val, "Value in the int_heap is differen from the input value");
}

void check_min_heap_clear_with_null(void) {
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_NULL_POINTER, min_heap_clear(NULL));
}
void check_min_heap_clear_return_value(void) {
    // The clear function resets only the size of the int_heap
    int_heap.size = 3;
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_OK, min_heap_clear(&int_heap));
}
void check_min_heap_clear_size(void) {
    // The clear function resets only the size of the int_heap
    int_heap.size = 3;
    min_heap_clear(&int_heap);
    TEST_ASSERT_EQUAL_size_t(int_heap.size, 0U);
}

void check_min_heap_insert_with_null_handler(void) {
    Point p = { .x = 5.4f, .y = 2.7f };
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_NULL_POINTER, min_heap_insert(NULL, &p));
}
void check_min_heap_insert_with_null_compare(void) {
    Point p = { .x = 5.4f, .y = 2.7f };
    point_heap.compare = NULL;
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_NULL_POINTER, min_heap_insert(&point_heap, &p));
}
void check_min_heap_insert_with_null_item(void) {
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_NULL_POINTER, min_heap_insert(&point_heap, NULL));
}
void check_min_heap_insert_when_full(void) {
    Point p = { .x = 5.4f, .y = 2.7f };
    point_heap.size = point_heap.capacity;
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_FULL, min_heap_insert(&point_heap, &p));
}
void check_min_heap_insert_return_value(void) {
    Point p = { .x = 5.4f, .y = 2.7f };
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_OK, min_heap_insert(&point_heap, &p));
}
void check_min_heap_insert_size(void) {
    Point p = { .x = 5.4f, .y = 2.7f };
    min_heap_insert(&point_heap, &p);
    TEST_ASSERT_EQUAL_size_t(1U, point_heap.size);
}
void check_min_heap_insert_top_data(void) {
    Point p = { .x = 5.4f, .y = 2.7f };
    min_heap_insert(&point_heap, &p);
    TEST_ASSERT_EQUAL_MEMORY(&p, point_heap.data, sizeof(Point));
}
void check_min_heap_insert_middle_data(void) {
    // Insert at the top a point that is greater then the next
    Point p1 = { .x = 5.4f, .y = 2.7f };
    Point p2 = { .x = 10.f, .y = 10.f };

    point_heap.size = 1;
    point_heap.data[0] = p2;

    min_heap_insert(&point_heap, &p1);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&p1, point_heap.data, sizeof(Point), "First item has different data");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&p2, point_heap.data + 1, sizeof(Point), "Second item has different data");
}

void check_min_heap_remove_with_null_handler(void) {
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_NULL_POINTER, min_heap_remove(NULL, 0, NULL));
}
void check_min_heap_remove_with_null_compare(void) {
    point_heap.compare = NULL;
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_NULL_POINTER, min_heap_remove(&point_heap, 0, NULL));
}
void check_min_heap_remove_when_empty(void) {
    TEST_ASSERT_EQUAL_INT(MIN_HEAP_EMPTY, min_heap_remove(&point_heap, 0, NULL));
}
void check_min_heap_remove_out_of_bounds(void) {
    Point p1 = { .x = 5.4f, .y = 2.7f };

    point_heap.size = 1;
    point_heap.data[0] = p1;

    TEST_ASSERT_EQUAL_INT(MIN_HEAP_OUT_OF_BOUNDS, min_heap_remove(&point_heap, 10, NULL));
}
void check_min_heap_remove_return_value(void) {
    Point p1 = { .x = 5.4f, .y = 2.7f };

    point_heap.size = 1;
    point_heap.data[0] = p1;

    TEST_ASSERT_EQUAL_INT(MIN_HEAP_OK, min_heap_remove(&point_heap, 0, NULL));
}
void check_min_heap_remove_removed_item_data(void) {
    Point p1 = { .x = 5.4f, .y = 2.7f };
    Point removed; 

    point_heap.size = 1;
    point_heap.data[0] = p1;

    min_heap_remove(&point_heap, 0, &removed);
    TEST_ASSERT_EQUAL_MEMORY(&p1, &removed, sizeof(Point));
}
void check_min_heap_remove_size(void) {
    Point p1 = { .x = 5.4f, .y = 2.7f };
    Point p2 = { .x = 10.f, .y = 10.f };

    point_heap.size = 2;
    point_heap.data[0] = p1;
    point_heap.data[1] = p2;

    min_heap_remove(&point_heap, 1, NULL);
    TEST_ASSERT_EQUAL_size_t(1, point_heap.size);
}
void check_min_heap_remove_leaf_data(void) {
    Point p1 = { .x = 5.4f, .y = 2.7f };
    Point p2 = { .x = 10.f, .y = 10.f };

    point_heap.size = 2;
    point_heap.data[0] = p1;
    point_heap.data[1] = p2;

    min_heap_remove(&point_heap, 1, NULL);
    // Check if other item is unchanged
    TEST_ASSERT_EQUAL_MEMORY(&p1, &point_heap.data[0], point_heap.data_size);
}
void check_min_heap_remove_equal_data(void) {
    Point root = { .x = 5.4f, .y = 2.7f };
    Point r = { .x = 5.4f, .y = 2.7f };
    Point l = { .x = 10.f, .y = 10.f };

    point_heap.size = 3;
    point_heap.data[0] = root;
    point_heap.data[1] = l;
    point_heap.data[2] = r;

    min_heap_remove(&point_heap, 0, NULL);
    TEST_ASSERT_EQUAL_MEMORY(&r, &point_heap.data[0], point_heap.data_size);
}
void check_min_heap_remove_up_heapify_data(void) {
    // The swapped element is lower than the last item in the heap (
    Point root = { .x = 1.f, .y = 1.f };
    Point l = { .x = 10.f, .y = 10.f };
    Point r = { .x = 1.4f, .y = 1.3f };

    Point ll = { .x = 11.f, .y = 11.f };
    Point lr = { .x = 12.f, .y = 12.f };
    Point rl = { .x = 2.f, .y = 2.f };
    Point rr = { .x = 3.14f, .y = 2.5f };

    point_heap.size = 7;
    point_heap.data[0] = root;
    point_heap.data[1] = l;
    point_heap.data[2] = r;
    point_heap.data[3] = ll;
    point_heap.data[4] = lr;
    point_heap.data[5] = rl;
    point_heap.data[6] = rr;


    // After the deletion of the 'll' node the 'l' and the 'rr' nodes should swap
    Point expected[6] = { root, rr, r, l, lr, rl };

    min_heap_remove(&point_heap, 3, NULL);
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, point_heap.data, sizeof(Point), 6);
}
void check_min_heap_remove_down_heapify_data(void) {
    // The swapped element is lower than the last item in the heap
    Point root = { .x = 1.f, .y = 1.f };
    Point l = { .x = 1.4f, .y = 1.3f };
    Point r = { .x = 10.f, .y = 10.f };

    Point ll = { .x = 2.f, .y = 2.f };
    Point lr = { .x = 3.14f, .y = 2.5f };
    Point rl = { .x = 11.f, .y = 11.f };
    Point rr = { .x = 12.f, .y = 12.f };

    point_heap.size = 7;
    point_heap.data[0] = root;
    point_heap.data[1] = l;
    point_heap.data[2] = r;
    point_heap.data[3] = ll;
    point_heap.data[4] = lr;
    point_heap.data[5] = rl;
    point_heap.data[6] = rr;

    // After the deletion of the 'root' node the 'll' and the 'rr' nodes should swap
    Point expected[6] = { l, ll, r, rr, lr, rl };

    min_heap_remove(&point_heap, 0, NULL);
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, point_heap.data, point_heap.data_size, 6);
}

void check_min_heap_find_with_null_heap(void) {
    int a = 0;
    TEST_ASSERT_LESS_THAN_INT(0, min_heap_find(NULL, &a));
}
void check_min_heap_find_with_null_item(void) {
    TEST_ASSERT_LESS_THAN_INT(0, min_heap_find(&int_heap, NULL));
}
void check_min_heap_find_when_empty(void) {
    int a = 0;
    TEST_ASSERT_LESS_THAN_INT(0, min_heap_find(&int_heap, &a));
}
void check_min_heap_find_fail(void) {
    int_heap.size = 3;
    int_heap.data[0] = 7;
    int_heap.data[1] = 3;
    int_heap.data[2] = 6;
    int a = 2;
    TEST_ASSERT_LESS_THAN_INT(0, min_heap_find(&int_heap, &a));
}
void check_min_heap_find_success(void) {
    int_heap.size = 3;
    int_heap.data[0] = 7;
    int_heap.data[1] = 3;
    int_heap.data[2] = 6;
    int a = 3;
    TEST_ASSERT_EQUAL_size_t(1, min_heap_find(&int_heap, &a));
}


int main() {
    UNITY_BEGIN();

    RUN_TEST(check_min_heap_init_with_null_handler);
    RUN_TEST(check_min_heap_init_with_null_callback);
    RUN_TEST(check_min_heap_init);

    RUN_TEST(check_min_heap_size_empty);
    RUN_TEST(check_min_heap_size_not_empty);

    RUN_TEST(check_min_heap_empty_with_null);
    RUN_TEST(check_min_heap_empty_when_empty);
    RUN_TEST(check_min_heap_empty_when_not_empty);

    RUN_TEST(check_min_heap_full_with_null);
    RUN_TEST(check_min_heap_full_when_full);
    RUN_TEST(check_min_heap_full_when_not_full);

    RUN_TEST(check_min_heap_top_with_null_heap);
    RUN_TEST(check_min_heap_top_with_null_item);
    RUN_TEST(check_min_heap_top_when_empty);
    RUN_TEST(check_min_heap_top_when_not_empty_return_value);
    RUN_TEST(check_min_heap_top_when_not_empty_data);

    RUN_TEST(check_min_heap_peek_with_null);
    RUN_TEST(check_min_heap_peek_when_empty);
    RUN_TEST(check_min_heap_peek_when_not_empty);

    RUN_TEST(check_min_heap_clear_with_null);
    RUN_TEST(check_min_heap_clear_return_value);
    RUN_TEST(check_min_heap_clear_size);

    RUN_TEST(check_min_heap_insert_with_null_handler);
    RUN_TEST(check_min_heap_insert_with_null_compare);
    RUN_TEST(check_min_heap_insert_with_null_item);
    RUN_TEST(check_min_heap_insert_when_full);
    RUN_TEST(check_min_heap_insert_return_value);
    RUN_TEST(check_min_heap_insert_size);
    RUN_TEST(check_min_heap_insert_top_data);
    RUN_TEST(check_min_heap_insert_middle_data);

    RUN_TEST(check_min_heap_remove_with_null_handler);
    RUN_TEST(check_min_heap_remove_with_null_compare);
    RUN_TEST(check_min_heap_remove_when_empty);
    RUN_TEST(check_min_heap_remove_out_of_bounds);
    RUN_TEST(check_min_heap_remove_return_value);
    RUN_TEST(check_min_heap_remove_removed_item_data);
    RUN_TEST(check_min_heap_remove_size);
    RUN_TEST(check_min_heap_remove_leaf_data);
    RUN_TEST(check_min_heap_remove_equal_data);
    RUN_TEST(check_min_heap_remove_up_heapify_data);
    RUN_TEST(check_min_heap_remove_down_heapify_data);

    RUN_TEST(check_min_heap_find_with_null_heap);
    RUN_TEST(check_min_heap_find_with_null_item);
    RUN_TEST(check_min_heap_find_when_empty);
    RUN_TEST(check_min_heap_find_fail);
    RUN_TEST(check_min_heap_find_success);

    UNITY_END();
}

