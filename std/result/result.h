#ifndef RESULT_H
#define RESULT_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

// GNU C extension: typeof on a compound literal to define the anonymous struct
// type
#ifdef __GNUC__
#define Result(T, E)                                                           \
  __typeof__((struct {                                                         \
    bool is_ok;                                                                \
    union {                                                                    \
      T ok;                                                                    \
      E err;                                                                   \
    } as;                                                                      \
  }){.is_ok = 0})
#else
#error "Result<T,E> requires GNU typeof extension"
#endif

// Construct an Ok(T) result
#define RESULT_OK(T, E, VAL) ((Result(T, E)){.is_ok = true, .as.ok = (VAL)})

// Construct an Err(E) result
#define RESULT_ERR(T, E, ERR) ((Result(T, E)){.is_ok = false, .as.err = (ERR)})

// Predicates
#define result_is_ok(R) ((R).is_ok)
#define result_is_err(R) (!(R).is_ok)

// Unwrap macros (assert on wrong variant)
#define unwrap(R)                                                              \
  ({                                                                           \
    assert(result_is_ok(R));                                                   \
    (R).as.ok;                                                                 \
  })
#define unwrap_err(R)                                                          \
  ({                                                                           \
    assert(result_is_err(R));                                                  \
    (R).as.err;                                                                \
  })

#endif // RESULT_H
