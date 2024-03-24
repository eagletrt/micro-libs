# Blinky handler

The blinky handler contains all the data needed to manage the led blinking pattern.

The `Blinky` handler structure contains the following fields:

- `pattern`: The user provided blinking pattern
- `t`: The timestamp of the last routine call (gets updated each routine call)
- `index`: The current index of the pattern array
- `size`: The length of the pattern
- `initial_state`: The initial state of the led
- `state`: The current state of the led
- `enable`: Flag to enable or disable the blinking
- `repeat`: Flag to repeat the pattern or just run once

!!! warning
    The handler should not be accessed or modified directly

