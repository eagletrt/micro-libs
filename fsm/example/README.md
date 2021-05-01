# Turnstiles

![fsm](turnstile.png)

This example implements a basic state machine that describes the functioning of a smartcard-operated turnstile.

It's reccomended to keep the fsm code in a separate file (`fsm_turnstile.c` in this example).

We have four states:

1. Init that initializes the peripherals?
2. Wait that waits for a smartcard and checks its validity.
3. Unlock that unlocks the turnstile for a limited amount of time.
4. Error that writes an error message on the display.

In `main.c` we only have to include `fsm_turnstile.c` and call two functions to make the state machine work.

To initialize the FSM, one just needs to fill `state_table` with the functions relative to each state.
