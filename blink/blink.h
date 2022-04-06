/**
 * @file	blink.h
 * @brief	Simple library to blink GPIOs without using timers or blocking code.
 *
 * @date	Sept 13, 2021
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "main.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#define BLINK_PATTERN_MAX_SIZE   64
#define _BLINK_ENABLE_MASK       0x80
#define _BLINK_REPEAT_MASK       0x40
#define _BLINK_PATTERN_SIZE_MASK 0x3F

struct blink {
    // GPIO definition
    GPIO_TypeDef *port;
    uint16_t pin;

    // Internal stuff
    uint32_t time;
    uint8_t index;

    // The pattern. Every cell of the array represents a time to wait. The GPIO is toggled after each cell.
    uint16_t pattern[BLINK_PATTERN_MAX_SIZE];
    uint8_t pattern_config;  // First bit: enable; Second bit: repeat; rest of byte: pattern size
};

typedef struct blink blink_t;

#define BLINK_GET_ENABLE(blink) (((blink).pattern_config & _BLINK_ENABLE_MASK) >> 7)
#define BLINK_SET_ENABLE(blink, enable) \
    ((blink).pattern_config = ((blink).pattern_config & (0xFF - _BLINK_ENABLE_MASK)) | ((enable == 1) << 7))

#define BLINK_GET_REPEAT(blink) (((blink).pattern_config & _BLINK_REPEAT_MASK) >> 6)
#define BLINK_SET_REPEAT(blink, repeat) \
    ((blink).pattern_config = ((blink).pattern_config & (0xFF - _BLINK_REPEAT_MASK)) | ((repeat == 1) << 6))

#define BLINK_GET_PATTERN_SIZE(blink) ((blink).pattern_config & _BLINK_PATTERN_SIZE_MASK)
#define BLINK_SET_PATTERN_SIZE(blink, size)                                                  \
    ((blink).pattern_config = ((blink).pattern_config & (0xFF - _BLINK_PATTERN_SIZE_MASK)) | \
                              (size & _BLINK_PATTERN_SIZE_MASK))

#define BLINK_SET_PATTERN(blink, blink_pattern, size)                          \
    {                                                                    \
        memcpy((blink).pattern, (blink_pattern), sizeof(*(blink_pattern)) * (size)); \
        BLINK_SET_PATTERN_SIZE(blink, size);                             \
    }

/**
 * @brief Resets the internal settings of the `blink_t` instance. Useful before starting a new pattern. It doesn't delete the pattern.
 * 
 * @param blink Blink instance to work with
 */
void blink_reset(blink_t *blink);

/**
 * @brief Blinks a GPIO
 * @details Call this function continuosly as this contains the timing logic.
 * 
 * @param blink Blink instance to work with
 */
void blink_run(blink_t *blink);
