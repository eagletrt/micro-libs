# Critical section

A **critical section** is defined as a region of code where some data is manipulated
and if an [interrupt](https://it.wikipedia.org/wiki/Interrupt) occurs its behaviour is
undefined and does not guarantee that the data is modified or read correctly.

To avoid undefined behaviours two functions are require by the ring buffer handler:
`cs_enter` and `cs_exit`, which are functions that are called at the start and at the end
of the critical section respectively.

Those functions are not present inside the library itself but their implementation
is left to the user, mainly because the implementation is often hardware dependant
and it is different from platform to platform.

