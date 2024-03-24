# BLINKY

This *light* library can be used to blink a digital LED following a custom pattern
without using a timer or blocking code.

> [!NOTE]
> This library does not actually blink the led directly but it manage its state internally

## Usage

To use this library first declare a new `Blinky` handler and initialize it with the
`blinky_init` function as follows:
```c
Blinky blinker;
blinky_init(&blinker, pattern, PATTERN_SIZE, true, BLINKY_LOW);
```

The init function requires a pattern with its length, the option to repeat it or run it once
and the initial state of the led.

To update the led status you need to call the `blinky_routine` function and set the actual
led status accordingly as follows:
```c
BlinkyState led_status = blinky_routine(&blinker, current_time);
set_led_status(led_status);
```
where the `current_time` is the time that the blinker library uses to check the led state
and the `set_led_status` function has to be implemented by the user (or substituted with
an equivalent function)

> [!INFO]
> The provided time defines the unit of measurement of the pattern (recomended is s or ms)

---

The pattern is an array of `uint16_t` where each element represent how much time
the led should remain in the current state. \
The initial state is set in the init function but can be changed any time.

If the difference between the current time and the previous saved time is greater than
the time specified in the pattern at the current index the state is **toggled** and the
index is incremented by one.

> [!NOTE]
> If the bilnker is set to repeat mode the index wraps around after it reaches the end,
> otherwise it is just set to `0`

For example with **HIGH** as initial state and the following pattern: `{ 500, 1000, 500 }`;
the led is kept **HIGH** for 500 units of time, then **LOW** for 1000
and then **HIGH** again for 500. \
If the pattern repeats, the state is reset to the initial state so, in this example,
after the led stays **HIGH** for 1000 units of time it stays **HIGH* for another 1000.

## Examples

Here is a complete example using an **STM32 microcontroller** with the **HAL library**:

```c
#include <stdint.h>
#include <stdbool.h>
#include "blinky.h"

#define BLINKY_PATTERN_SIZE 4

Blinky blinker;
uint16_t pattern[BLINKY_PATTERN_SIZE] = {
    500,
    500,
    500,
    500
};

int main(void) {
    // Init blinker
    blinky_init(&blinker, pattern, BLINKY_PATTERN_SIZE, true, BLINKY_LOW);

    while (1) {
        // Run blinker and update the led
        BlinkyState led_status = blinky_routine(&blinker, HAL_GetTick());
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, led_status);
        HAL_Delay(10); // Optional, not needed
    }
}
```

