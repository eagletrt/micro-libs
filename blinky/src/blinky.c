/**
 * @file blinky.c
 * @brief Blink 
 *
 * @date 14 Dec 2023
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 */

#include "blinky.h"

void blinky_init(
    Blinky * b,
    uint16_t * pattern,
    uint8_t size,
    bool repeat)
{
    if (b == NULL) return;
    b->pattern = pattern;
    b->t = 0;
    b->index = 0;
    b->size = size;
    b->state = BLINKY_HIGH;
    b->enable = true;
    b->repeat = repeat;
}

void blinky_enable(Blinky * b, bool enabled) {
    if (b == NULL) return;
    b->enable = enabled;
}

void blinky_repeat(Blinky * b, bool repeat) {
    if (b == NULL) return;
    b->repeat = repeat;
}

void blinky_set_pattern(Blinky * b, uint16_t * pattern) {
    if (b == NULL) return;
    b->pattern = pattern;
}

void blinky_reset(Blinky * b) {
    if (b == NULL) return;
    b->t = 0;
    b->index = 0;
    b->state = BLINKY_HIGH;
    b->enable = true;
}

BlinkyState blinky_routine(Blinky * b, uint32_t t) {
    if (b == NULL || b->pattern == NULL) return BLINKY_LOW;
    if (!b->enable) return BLINKY_LOW;

    // Check if state has to be changed
    if (t - b->t >= b->pattern[b->index]) {
        b->state = !b->state;
        b->t = t;
        ++b->index;

        // Wrap around if pattern ended
        if (b->index >= b->size) {
            // Disable if repeat is disabled
            if (!b->repeat)
                b->enable = false;

            b->index = 0;
            b->state = BLINKY_HIGH;
        }
    }

    return b->state;
}

