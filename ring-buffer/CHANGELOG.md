# Change Log

## 22-03-2024

### Added

- Init function that can be called after the buffer declaration instead of the initializer macro
- Dummy function that is called if the critical section handler functions are not given by the user
  to avoid NULL checking (the function is empty)

### Changed

- Changed `ring_buffer_init` macro name to `ring_buffer_new`
- Give critical section handler functions as a parameter to the initializer instead
  of defining them with a specific name

