/**
 * @file	blink.c
 * @brief	Simple library to blink GPIOs without using timers or blocking code.
 *
 * @date	Sept 13, 2021
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "blink.h"

void blink_reset(blink_t *blink) {
    blink->index = 0;
    blink->time  = HAL_GetTick();
}

void blink_run(blink_t *blink) {
    if (BLINK_GET_ENABLE(*blink)) {
        if (blink->index >= BLINK_GET_PATTERN_SIZE(*blink)) {
            blink->index = 0;
            if (!BLINK_GET_REPEAT(*blink)) {
                // If `repeat` is deactivated, then the blink is deactivated too.
                BLINK_SET_ENABLE(*blink, false);
            }
        }
        if (HAL_GetTick() - blink->time >= blink->pattern[blink->index]) {
            HAL_GPIO_TogglePin(blink->port, blink->pin);
            blink->time = HAL_GetTick();
            blink->index++;
        }
    }
}
