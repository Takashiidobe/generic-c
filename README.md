# Generic C

Notes from:
https://danielchasehooper.com/posts/typechecked-generic-c-data-structures/

Data structures built in the generic C style that the author suggests.

## Building

Requires a compiler with GNU extensions and C23 support (clang or gcc,
`-std=gnu2x`) — the containers rely on `typeof`, statement expressions, and
`__auto_type`. Build and test with [`just`](https://github.com/casey/just):

```sh
just            # compile + run every main.c under clang, strict -Werror
just test-gcc   # same, with gcc
just asan       # run under AddressSanitizer + UndefinedBehaviorSanitizer
just lint       # aggressive clang warnings (non-fatal; reserved-id, cast-align)
just clean
```
