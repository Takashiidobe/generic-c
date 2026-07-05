#ifndef GC_ASSERT_H
#define GC_ASSERT_H

#include <stdio.h>
#include <stdlib.h>

[[noreturn]] static inline void gc_assert_fail(const char *expression,
                                               const char *file, int line) {
  fprintf(stderr, "%s:%d: assertion failed: %s\n", file, line, expression);
  abort();
}

#define GC_ASSERT(expression)                                                  \
  do {                                                                         \
    if (!(expression))                                                         \
      gc_assert_fail(#expression, __FILE__, __LINE__);                         \
  } while (0)

#endif
