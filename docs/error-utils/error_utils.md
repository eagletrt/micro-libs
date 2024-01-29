# Error utils

The main purpose of this library is to handle **critical errors** that should
expire after a given time from the moment they were set.

A critical error is defined as an abnormal situation that should be notified to
the system and can be uniquely identified by a **type**, defined by the user,
and an **instance** that should identify the cause of the error.

The error instance can be a constant string (useful to give meaning to the instance)
or an integer (useful to enumarate and iterate multiple instances).

There are three main operations that can be done with an error:
1. **Set**
2. **Reset**
3. **Expire**

If an error is set the current *timestamp* is saved and an expire handler (usually a timer)
is started or updated.
If a running error is reset it cannot expire anymore, in that case the expire handler
is updated or stopped if there are no running errors.
If an error expires the system is notified through a callback, the expired error
is reset and the expire handler is updated or stopped.

!!! Important The exipre handler has to be defined and handled by the user,
this library only defines a couple of interfaces and does not manage
the handler internally.

---

The internal structure of the error utils library takes advantage of **two data
structures** to achieve fast insertion, deletion and search:
1. [Hash table](data_structures/hash_table.md): to handle error insertion(*set*)
and deletion(*reset*)
2. [Min heap](data_structures/heap.md): to handle the expire mechanism and sort
the errors based on their expiration time
