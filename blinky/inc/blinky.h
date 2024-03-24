/**
 * @file blinky.h
 * @brief Blink a digital led following a given pattern
 * without using timers or blocking code
 *
 * @date 14 Dec 2023
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#ifndef BLINKY_H
#define BLINKY_H

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    BLINKY_LOW = 0,
    BLINKY_HIGH
} BlinkyState;

/**
 * !!! GO, SLINKY GO !!!
 *
 * @attention Do not reorder elements, with this order
 * the size occupied by the struct is the smallest possible.
 * This is due to allignment of the struct bytes
 * 
 * @detail The time unit of measurement is dictated by the time passed to the functions (usually milliseconds)
 *
 * @param pattern An array of numbers representing the amount of time the led
 *        should remain in the current state.
 *        The state of the led starts as high and it toggles for every value of the array
 * @param t Last routine call timestamp
 * @param index The index of the current pattern value
 * @param size The number of elements in the pattern
 * @param state The current state of the led
 * @param enable Enable or disable blinking
 * @param repeat Run once or repeat
 */
typedef struct {
    uint16_t * pattern;
    uint32_t t;
    uint8_t index;
    uint8_t size;
    BlinkyState initial_state : 1;
    BlinkyState state : 1;
    bool enable : 1;
    bool repeat : 1;
} Blinky;


/**
 * @brief Initialize blink
 *
 * @param b The blinky handler
 * @param pattern A pointer to the pattern to execute
 * @param size The length of the pattern
 * @param repeat Wheter to repeat the patter or run only once
 * @param state The initial state of the led
 */
void blinky_init(
    Blinky * b,
    uint16_t * pattern,
    uint8_t size,
    bool repeat,
    BlinkyState state
);

/**
 * @brief Enable or disable blinking
 *
 * @param b The blinky handler
 * @param enabled True to enable blinky, false to disable
 */
void blinky_enable(Blinky * b, bool enabled);

/**
 * @brief Enable or disable pattern repetition
 * 
 * @param b The blinky handler
 * @param repeat True to repeat, false to run once
 */
void blinky_repeat(Blinky * b, bool repeat);

/**
 * @brief Set a new patter to execute
 *
 * @param b The blinky handler
 * @param pattern The new pattern to set
 * @param size The length of the pattern
 */
void blinky_set_pattern(Blinky * b, uint16_t * pattern, uint8_t size);

/**
 * @brief Re-enable and reset blinker
 * @details The repeat option does not change
 *
 * @param b The blinky handler
 * @param state The inital state of the led
 */
void blinky_reset(Blinky * b, BlinkyState state);

/**
 * @brief Routine to check the state of the led
 *
 * @param b The blinky handler
 * @param t The current time
 * @return BlinkyState The current state of the led
 */
BlinkyState blinky_routine(Blinky * b, uint32_t t);

#endif  // BLINKY_H

