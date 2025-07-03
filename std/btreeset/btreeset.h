#ifndef BTREESET_H
#define BTREESET_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// —— internal header: length, capacity, and raw data array ——
typedef struct {
  size_t len, cap;
  char data[]; // floor(cap) slots of [ key_bytes ]
} SSetHeader;

// —— declare a sorted‐set variable for element type E ——
#define BSet(E)                                                                \
  struct {                                                                     \
    SSetHeader *h;                                                             \
    E _key;                                                                    \
  }

// —— helpers ——
// number of elements
#define ss_len(s) ((s).h ? (s).h->len : 0)
// storage stride = sizeof(key)
#define _ss_stride(s) (sizeof((s)._key))
// header size up to data[]
#define _ss_hdr(s) (offsetof(SSetHeader, data))
// raw byte pointer
#define _ss_data(s) ((char *)(s).h->data)
// pointer to element slot i
#define _ss_keyptr(s, i)                                                       \
  ((typeof((s)._key) *)(_ss_data(s) + (i) * _ss_stride(s)))

// —— initialize & free ——
#define bset_init(s, init_cap)                                                 \
  do {                                                                         \
    size_t __stride = _ss_stride(s);                                           \
    size_t __hdr = _ss_hdr(s);                                                 \
    (s).h = malloc(__hdr + __stride * (init_cap));                             \
    assert((s).h);                                                             \
    (s).h->len = 0;                                                            \
    (s).h->cap = (init_cap);                                                   \
  } while (0)

#define bset_free(s)                                                           \
  do {                                                                         \
    if ((s).h) {                                                               \
      free((s).h);                                                             \
      (s).h = NULL;                                                            \
    }                                                                          \
  } while (0)

// —— grow capacity when full ——
#define _ss_ensure(s)                                                          \
  do {                                                                         \
    SSetHeader *__sh = (s).h;                                                  \
    if (__sh->len >= __sh->cap) {                                              \
      size_t __newc = __sh->cap ? __sh->cap * 2 : 1;                           \
      size_t __stride = _ss_stride(s);                                         \
      size_t __hdr = _ss_hdr(s);                                               \
      __sh = realloc(__sh, __hdr + __stride * __newc);                         \
      assert(__sh);                                                            \
      __sh->cap = __newc;                                                      \
      (s).h = __sh;                                                            \
    }                                                                          \
  } while (0)

// —— insert element (no‐op if already present) ——
#define bset_add(s, key_expr)                                                  \
  do {                                                                         \
    if (!(s).h)                                                                \
      bset_init(s, 1);                                                         \
    typeof((s)._key) __k = (key_expr);                                         \
    SSetHeader *__sh = (s).h;                                                  \
    size_t __n = __sh->len, __lo = 0, __hi = __n;                              \
    bool __found = false;                                                      \
                                                                               \
    /* binary search for insertion point */                                    \
    while (__lo < __hi) {                                                      \
      size_t __mid = (__lo + __hi) >> 1;                                       \
      typeof((s)._key) __mk = *_ss_keyptr(s, __mid);                           \
      if (__k < __mk)                                                          \
        __hi = __mid;                                                          \
      else if (__k > __mk)                                                     \
        __lo = __mid + 1;                                                      \
      else {                                                                   \
        __found = true;                                                        \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    if (!__found) {                                                            \
      _ss_ensure(s);                                                           \
      __sh = (s).h;                                                            \
      __n = __sh->len;                                                         \
      /* shift right to make room at index __lo */                             \
      size_t __stride = _ss_stride(s);                                         \
      char *__base = _ss_data(s);                                              \
      memmove(__base + (__lo + 1) * __stride, __base + (__lo) * __stride,      \
              (__n - __lo) * __stride);                                        \
      /* insert new element */                                                 \
      *_ss_keyptr(s, __lo) = __k;                                              \
      __sh->len = __n + 1;                                                     \
    }                                                                          \
  } while (0)

// —— test membership ——
#define bset_contains(s, key_expr)                                             \
  ({                                                                           \
    bool __hit = false;                                                        \
    if ((s).h) {                                                               \
      typeof((s)._key) __k = (key_expr);                                       \
      SSetHeader *__sh = (s).h;                                                \
      size_t __lo = 0, __hi = __sh->len;                                       \
      while (__lo < __hi) {                                                    \
        size_t __mid = (__lo + __hi) >> 1;                                     \
        typeof((s)._key) __mk = *_ss_keyptr(s, __mid);                         \
        if (__k < __mk)                                                        \
          __hi = __mid;                                                        \
        else if (__k > __mk)                                                   \
          __lo = __mid + 1;                                                    \
        else {                                                                 \
          __hit = true;                                                        \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    __hit;                                                                     \
  })

// —— iterate in sorted order ——
#define bset_for(s, it)                                                        \
  for (size_t __i = 0, __n = ss_len(s); __i < __n; ++__i)                      \
    for (int __once = 1; __once; __once = 0)                                   \
      for (typeof((s)._key) it = *_ss_keyptr(s, __i); __once; __once = 0)

#endif // BTREESET_H
