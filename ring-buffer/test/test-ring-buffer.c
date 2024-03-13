/**
 * @file test-ring-buffer.c
 * @brief Unit test functions for the circular buffer library
 *
 * @date 13 Mar 2024
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#include "unity.h"
#include "ring-buffer.h"

typedef struct {
    int x, y;
} Point;

RingBuffer(int, 10) int_buf = ring_buffer_init(int, 10);
RingBuffer(Point, 10) point_buf = ring_buffer_init(Point, 10);

void setUp(void) {

}

void tearDown(void) {
    ring_buffer_clear(int_buf);
    ring_buffer_clear(int_buf);
}

int main() {
    UNITY_BEGIN();


    UNITY_END();
}

