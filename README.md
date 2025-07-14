# Red Black tree in C

An implementation of Red Black tree in C, inspired by [PostgreSQL implementation](https://doxygen.postgresql.org/rbtree_8c_source.html).

Tree key can be set by defining macro, for example,  `RBTREE_KEY(name) uint32_t key[1]`
and `RBTREE_KEY_BASE_TYPE uint32_t` which would use 32 bits unsigned integer for
the key. Use gcc argument to make that change : `-D"RBTREE_KEY(name)"="uint32_t
name[2]" -DRBTREE_KEY_BASE_TYPE="uint32_t"`.
For simple value, like `uint32_t` as key, use `uint32_t [1]` or else it will segfault.

By default it is defined as `uint64_t key[2]` as it was originally
designed to hold [UUID](https://en.wikipedia.org/wiki/Universally_unique_identifier).

Data part of a node is `uintptr_t` so it can hold either a integer or a pointer.

It implements basic operation : insert, search, delete, left most and
right most. So it can be used for an in-memory index implementation.
