# Operations

The blinky library has a few functions that can change the configuration and
behaviour of the blinking pattern.

You can **enable** or **disable** the blinking handler any time but keep in mind that
neither of those operations resets the internal status of the handler.

The pattern repetition behaviour can be enabled or disabled any time and, if the handler
is configured to run just once, the `enable` is set to `false` after the end of the pattern.

The pattern can also be changed at runtime whithout reseting the internal state of the handler.

Finally you can restore the initial state of the handler.

## Blinking routine

The routine function should be called **as often as possible** and its task is to constantly
update the state of the led to match the specified pattern.

If the handler or the pattern are `NULL` the routine always return the `BLINKY_LOW` state
and if it is not enabled it just return the last state of the led.

Otherwise this function checks if the elapsed time from the last state change is greater
or equal to the interval specified inside the pattern at the current index;
if so it updates the led state, the time and the pattern index.

If the next index is greater or equal than the length of the pattern the handler is disabled
only if the pattern does not have to repeat and the index and state are reset to the initial
values.

