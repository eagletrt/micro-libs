# Change Log

## 14-04-2024

### Added

- Makefile utility file

## 13-04-2024

### Added

- Enum with the return codes for the min heap functions
- NULL checks for compare function

### Changed

- Changed the following functions return values to the `MinHeapReturnCode` enum:
  - `min_heap_init`
  - `min_heap_insert`
  - `min_heap_remove`
  - `min_heap_top`
  - `min_heap_clear`

## 22-03-2024

### Added

- Init function that can be called after the heap declaration instead of the initializer macro

### Changed

- Changed `min_heap_init` macro to `min_heap_new`

