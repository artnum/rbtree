# Red Black tree in C

An implementation of Red Black tree in C, inspired by [PostgreSQL implementation](https://doxygen.postgresql.org/rbtree_8c_source.html).

Tree key can be set by defining macro, for example,  `RBTREE_KEY uint32_t key`
and `RBTREE_KEY_BASE_TYPE uint32_t` which would use 32 bits unsigned integer for
the key. By default it is defined as `uint64_t key[2]` as it was originally
designed to hold [UUID](https://en.wikipedia.org/wiki/Universally_unique_identifier).

Data part of a node is `uintptr_t` so it can hold either a integer or a pointer.

It implements basic operation : insert, search and delete. So it can be used
for an in-memory index implementation.
