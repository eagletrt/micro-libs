# BLINKY

This *light* library can be used to blink a digital LED following a custom pattern
without using a timer or blocking code.

## Usage

To use this library compile and link everything inside the `src` folder,
and add the `inc` folder to the include path of your project.

Define a variable of type `Blinky` and initialize it with the `blinky_init` passing
the pattern, the pattern size and if it should run once or repeatedly.

Then run the `blinky_routine` repeatedly providing the time to get the current state of the led,
and set the *real* led state accordingly.

> [!NOTE]
> The provided time defines the unit of measurement of the pattern (recomended is s or ms)

---

The pattern is an array of `uint16_t` where each element represent how much time
the led should remain in the current state. \
The initial state is **HIGH** and it toggles for every element of the array.

For example a pattern of `{ 500, 1000, 500 }` keeps the led state **HIGH** for 500
unit of time, **LOW** for 1000 and then **HIGH** again for 500. \
If the pattern repeats, the state is set to **HIGH** independently from the last state of the led,
so, in this example, the led remains in the **HIGH** state for 1000 unit of time.

## Development

If changes are made to this library, the README, the documentation and the test files should be updated accordingly.

Every time a change is made run `make test_all` in the `test` folder to ensure that
everything is working correctly. \
If at least one test fails or the program crash, solve the problem and re-run the test
until each one of them is passed.

