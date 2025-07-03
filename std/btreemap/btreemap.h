// bmap.h
#ifndef btreemap_h
#define btreemap_h

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// —— internal header: length, capacity, and raw data array ——
typedef struct {
  size_t len, cap;
  char data[]; // floor(cap) slots of [ key_bytes | val_bytes ]
} SMapHeader;

// —— declare a sorted‐map variable for key type K and value type V ——
#define BMap(K, V)                                                             \
  struct {                                                                     \
    SMapHeader *h;                                                             \
    K _key;                                                                    \
    V _val;                                                                    \
  }

// —— helpers ——
// number of elements
#define sm_len(m) ((m).h ? (m).h->len : 0)
// storage stride = sizeof(key)+sizeof(val)
#define _sm_stride(m) (sizeof((m)._key) + sizeof((m)._val))
// raw byte pointer
#define _sm_data(m) ((char *)(m).h->data)
// pointer to key slot i
#define _sm_keyptr(m, i)                                                       \
  ((typeof((m)._key) *)(_sm_data(m) + (i) * _sm_stride(m)))
// pointer to val slot i
#define _sm_valptr(m, i)                                                       \
  ((typeof((m)._val) *)(_sm_data(m) + (i) * _sm_stride(m) + sizeof((m)._key)))

// —— initialize & free ——
#define bmap_init(m, init_cap)                                                 \
  do {                                                                         \
    size_t __stride = _sm_stride(m);                                           \
    size_t __hdr = offsetof(SMapHeader, data);                                 \
    (m).h = malloc(__hdr + __stride * (init_cap));                             \
    assert((m).h);                                                             \
    (m).h->len = 0;                                                            \
    (m).h->cap = (init_cap);                                                   \
  } while (0)

#define bmap_free(m)                                                           \
  do {                                                                         \
    if ((m).h) {                                                               \
      free((m).h);                                                             \
      (m).h = NULL;                                                            \
    }                                                                          \
  } while (0)

// —— grow capacity when full ——
#define _sm_ensure(m)                                                          \
  do {                                                                         \
    SMapHeader *__sh = (m).h;                                                  \
    if (__sh->len >= __sh->cap) {                                              \
      size_t __newc = __sh->cap ? __sh->cap * 2 : 1;                           \
      size_t __stride = _sm_stride(m);                                         \
      size_t __hdr = offsetof(SMapHeader, data);                               \
      __sh = realloc(__sh, __hdr + __stride * __newc);                         \
      assert(__sh);                                                            \
      __sh->cap = __newc;                                                      \
      (m).h = __sh;                                                            \
    }                                                                          \
  } while (0)

// —— insert or overwrite ——
#define bmap_put(m, key_expr, val_expr)                                        \
  do {                                                                         \
    if (!(m).h)                                                                \
      bmap_init(m, 1);                                                         \
    typeof((m)._key) __k = (key_expr);                                         \
    typeof((m)._val) __v = (val_expr);                                         \
    SMapHeader *__sh = (m).h;                                                  \
    size_t __n = __sh->len, __lo = 0, __hi = __n;                              \
    bool __found = false;                                                      \
                                                                               \
    /* binary search */                                                        \
    while (__lo < __hi) {                                                      \
      size_t __mid = (__lo + __hi) >> 1;                                       \
      typeof((m)._key) __mk = *_sm_keyptr(m, __mid);                           \
      if (__k < __mk)                                                          \
        __hi = __mid;                                                          \
      else if (__k > __mk)                                                     \
        __lo = __mid + 1;                                                      \
      else {                                                                   \
        *_sm_valptr(m, __mid) = __v;                                           \
        __found = true;                                                        \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    if (!__found) {                                                            \
      _sm_ensure(m);                                                           \
      __sh = (m).h;                                                            \
      __n = __sh->len;                                                         \
      __lo = __lo;                                                             \
      size_t __stride = _sm_stride(m);                                         \
      char *__base = _sm_data(m);                                              \
                                                                               \
      /* shift right to make room */                                           \
      memmove(__base + (__lo + 1) * __stride, __base + __lo * __stride,        \
              (__n - __lo) * __stride);                                        \
                                                                               \
      *_sm_keyptr(m, __lo) = __k;                                              \
      *_sm_valptr(m, __lo) = __v;                                              \
      __sh->len = __n + 1;                                                     \
    }                                                                          \
  } while (0)

// —— lookup: returns pointer to V or NULL ——
#define bmap_get(m, key_expr)                                                  \
  ({                                                                           \
    typeof((m)._val) *__res = NULL;                                            \
    if ((m).h) {                                                               \
      typeof((m)._key) __k = (key_expr);                                       \
      SMapHeader *__sh = (m).h;                                                \
      size_t __lo = 0, __hi = __sh->len;                                       \
      while (__lo < __hi) {                                                    \
        size_t __mid = (__lo + __hi) >> 1;                                     \
        typeof((m)._key) __mk = *_sm_keyptr(m, __mid);                         \
        if (__k < __mk)                                                        \
          __hi = __mid;                                                        \
        else if (__k > __mk)                                                   \
          __lo = __mid + 1;                                                    \
        else {                                                                 \
          __res = _sm_valptr(m, __mid);                                        \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    __res;                                                                     \
  })

// —— iterate in sorted order ——
#define bmap_for(m, it_k, it_v)                                                \
  for (size_t __i = 0, __n = sm_len(m); __i < __n; ++__i)                      \
    for (int __once1 = 1; __once1; __once1 = 0)                                \
      for (typeof((m)._key) it_k = *_sm_keyptr(m, __i); __once1; __once1 = 0)  \
        for (int __once2 = 1; __once2; __once2 = 0)                            \
          for (typeof((m)._val) it_v = *_sm_valptr(m, __i); __once2;           \
               __once2 = 0)

#endif // btreemap_h
