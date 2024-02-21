# Hash table

One of the main problems with how the errors are defined is that their total number
cannot be set from inside the library and dynamic memory allocation should be avoided.

To solve this problems an **hash table** with **open addressing** is used.

!!! Info The hash function used by the library is the 32 bit version of the
**MurmurHash3** implementation, for details refer to [MurmurHash](https://en.wikipedia.org/wiki/MurmurHash)

When a new error is set its hash value is calculated based on the type and its
instance, then the calculated value is used as the index for the hash table. \
If a collision happens the next index is calculated using **quadratic probing**
as described by this formula: `next_index = (curr_index + (offset * offset)) % buffer_size`
where the `offset` is the number of collisions and the `buffer_size` variable
is the size of the hash table.

When an error is reset it's not removed completely from the hash table but can
be replaced with another when needed.

The advantages of the MurmurHash algorithm is that it takes as input a 32 bit
integer **seed** (which is the error type in our case) and a **stream of bytes**
(which is the error instance), and it is suitable for generic hash-based lookup
like in the case of this library.
