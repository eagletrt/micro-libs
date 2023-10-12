# Heap

To handle the expiration mechanism of the errors it is needed to get the error
with the **minimum remaining amount of time** before it expire, keeping it possible
to do fast insertion and removal.

One possible solution would be to sort each error every time it is set or reset
but, in the worst case scenario, it could take a lot of time to do that.

To solve this problem a separate data structure containing pointer to the errors
is **partially sorted** keeping the minimum element at the first index of the structure;
this structure is a [min heap](https://it.wikipedia.org/wiki/Heap_(struttura_dati))
with a static array implementation.

When an error is set a reference to that error is pushed at the end of the heap
and than it is swapped with its parent until it restores the properties of the heap.

When an error is reset the referenced to that error is swapped with the last element
of the heap and removed, and then the properties of the heap are restored.

Every time the first element (the minimum) is updated the expire handler is also
updated as well, except for the case in which no errors are running where the
expire handler is stopped.
