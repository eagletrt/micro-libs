# Change Log

## 14-04-2024

### Added

- Makefile utility file

## 13-04-2024

### Added

- Enum with the return codes for the ring buffer functions

### Changed

- Changed the following functions return values to the `RingBufferReturnCode` enum:
  - `ring_buffer_init`
  - `ring_buffer_push_front`
  - `ring_buffer_push_back`
  - `ring_buffer_pop_front`
  - `ring_buffer_pop_back`
  - `ring_buffer_front`
  - `ring_buffer_back`
  - `ring_buffer_clear`

## 22-03-2024

### Added

- Init function that can be called after the buffer declaration instead of the initializer macro
- Dummy function that is called if the critical section handler functions are not given by the user
  to avoid NULL checking (the function is empty)

### Changed

- Changed `ring_buffer_init` macro name to `ring_buffer_new`
- Give critical section handler functions as a parameter to the initializer instead
  of defining them with a specific name

