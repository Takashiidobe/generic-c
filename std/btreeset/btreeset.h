#ifndef BTREESET_H
#define BTREESET_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../hash/hash.h"

// —— internal header: length, capacity, comparator, and raw data array ——
// cmp == NULL means order elements with the built-in < / > operators (scalars);
// set a comparator (e.g. gc_str_cmp) via bset_init_with for other element types.
typedef struct {
  size_t len, cap;
  gc_cmp_fn cmp;
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
// three-way compare of element __k against slot i, honoring a custom comparator
#define _ss_cmp(s, sh, k, i)                                                   \
  ((sh)->cmp ? (sh)->cmp(&(k), _ss_keyptr(s, i), sizeof((s)._key))             \
             : ((k) < *_ss_keyptr(s, i) ? -1                                   \
                                        : ((k) > *_ss_keyptr(s, i) ? 1 : 0)))

// —— initialize with a custom comparator (use gc_str_cmp for string elems) ——
#define bset_init_with(s, init_cap, cmp_fn)                                    \
  do {                                                                         \
    size_t __stride = _ss_stride(s);                                           \
    size_t __hdr = _ss_hdr(s);                                                 \
    (s).h = malloc(__hdr + __stride * (init_cap));                             \
    assert((s).h);                                                             \
    (s).h->len = 0;                                                            \
    (s).h->cap = (init_cap);                                                   \
    (s).h->cmp = (cmp_fn);                                                     \
  } while (0)

// —— initialize & free (built-in scalar ordering) ——
#define bset_init(s, init_cap) bset_init_with(s, init_cap, NULL)

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
      int __c = _ss_cmp(s, __sh, __k, __mid);                                  \
      if (__c < 0)                                                             \
        __hi = __mid;                                                          \
      else if (__c > 0)                                                        \
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
        int __c = _ss_cmp(s, __sh, __k, __mid);                                \
        if (__c < 0)                                                           \
          __hi = __mid;                                                        \
        else if (__c > 0)                                                      \
          __lo = __mid + 1;                                                    \
        else {                                                                 \
          __hit = true;                                                        \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    __hit;                                                                     \
  })

// —— remove an element: returns 1 if it was present, 0 otherwise ——
#define bset_remove(s, key_expr)                                               \
  ({                                                                           \
    int __removed = 0;                                                         \
    if ((s).h) {                                                               \
      typeof((s)._key) __k = (key_expr);                                       \
      SSetHeader *__sh = (s).h;                                                \
      size_t __lo = 0, __hi = __sh->len;                                       \
      while (__lo < __hi) {                                                    \
        size_t __mid = (__lo + __hi) >> 1;                                     \
        int __c = _ss_cmp(s, __sh, __k, __mid);                                \
        if (__c < 0)                                                           \
          __hi = __mid;                                                        \
        else if (__c > 0)                                                      \
          __lo = __mid + 1;                                                    \
        else {                                                                 \
          size_t __stride = _ss_stride(s);                                     \
          char *__base = _ss_data(s);                                          \
          memmove(__base + __mid * __stride,                                   \
                  __base + (__mid + 1) * __stride,                             \
                  (__sh->len - __mid - 1) * __stride);                         \
          __sh->len--;                                                         \
          __removed = 1;                                                       \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    __removed;                                                                 \
  })

// —— iterate in sorted order ——
#define bset_for(s, it)                                                        \
  for (size_t __i = 0, __n = ss_len(s); __i < __n; ++__i)                      \
    for (int __once = 1; __once; __once = 0)                                   \
      for (typeof((s)._key) it = *_ss_keyptr(s, __i); __once; __once = 0)

#endif // BTREESET_H
