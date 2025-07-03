#ifndef RESULT_H
#define RESULT_H

#include <assert.h>
#include <stdbool.h>

// Define a named Result<T, E> type. Mirrors DEFINE_OPTION: each anonymous
// `typeof` struct is a distinct, incompatible type, so a named typedef is
// required for values to pass across function boundaries.
//
//   DEFINE_RESULT(ParseResult, int, const char *);
//   ParseResult r = RESULT_OK(ParseResult, 5);
#define DEFINE_RESULT(Name, T, E)                                              \
  typedef struct Name {                                                        \
    bool is_ok;                                                                \
    union {                                                                    \
      T ok;                                                                    \
      E err;                                                                   \
    } as;                                                                      \
  } Name

// Construct an Ok(T) result
#define RESULT_OK(Name, VAL) ((Name){.is_ok = true, .as.ok = (VAL)})

// Construct an Err(E) result
#define RESULT_ERR(Name, ERR) ((Name){.is_ok = false, .as.err = (ERR)})

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
