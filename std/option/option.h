// option.h
#ifndef OPTION_H
#define OPTION_H

#include <assert.h>
#include <stdbool.h>

/* ————— Define a new Option<T> type —————
 *
 * After you write:
 *   DEFINE_OPTION(IntPair);
 *
 * you get a real typedef:
 *
 *   typedef struct {
 *     bool   is_some;
 *     union { IntPair some; } as;
 *   } Option_IntPair;
 *
 * Then you can write `Option(IntPair)` and the macros below will
 * all target that named struct type.
 */
#define DEFINE_OPTION(T)                                                       \
  typedef struct {                                                             \
    bool is_some;                                                              \
    union {                                                                    \
      T some;                                                                  \
    } as;                                                                      \
  } Option_##T;

/* ————— “Option<T>” is just the named type Option_T ————— */
#define Option(T) Option_##T

/* ————— Construct a Some or None ————— */
#define OPTION_SOME(T, VAL) ((Option(T)){.is_some = true, .as.some = (VAL)})

#define OPTION_NONE(T) ((Option(T)){.is_some = false})

/* ————— Predicates ————— */
#define option_is_some(O) ((O).is_some)
#define option_is_none(O) (!(O).is_some)

/* ————— Unwrap ————— */
#define unwrap_some(O)                                                         \
  ({                                                                           \
    assert(option_is_some(O));                                                 \
    (O).as.some;                                                               \
  })
#define unwrap_or(O, D) ({ option_is_some(O) ? (O).as.some : (D); })

#endif // OPTION_H
