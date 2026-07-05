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
// set a comparator (e.g. gc_str_cmp) via bset_init_with for other element
// types.
typedef struct {
  size_t len, cap;
  gc_cmp_fn cmp;
  max_align_t data[];
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
#define gc_ss_stride(s) (sizeof((s)._key))
// header size up to data[]
#define gc_ss_hdr(s) (offsetof(SSetHeader, data))
// raw byte pointer
#define gc_ss_data(s) ((char *)(s).h->data)
// pointer to element slot i
#define gc_ss_keyptr(s, i)                                                     \
  ((typeof((s)._key) *)(void *)(gc_ss_data(s) + (i) * gc_ss_stride(s)))
// three-way compare of element gc_k against slot i, honoring a custom
// comparator
#define gc_ss_cmp(s, sh, k, i)                                                 \
  ((sh)->cmp                                                                   \
       ? (sh)->cmp(&(k), gc_ss_keyptr(s, i), sizeof((s)._key))                 \
       : ((k) < *gc_ss_keyptr(s, i) ? -1                                       \
                                    : ((k) > *gc_ss_keyptr(s, i) ? 1 : 0)))

// —— initialize with a custom comparator (use gc_str_cmp for string elems) ——
#define bset_init_with(s, init_cap, cmp_fn)                                    \
  do {                                                                         \
    static_assert(_Alignof(typeof((s)._key)) <= _Alignof(max_align_t),         \
                  "bset element alignment exceeds max_align_t");               \
    size_t gc_stride = gc_ss_stride(s);                                        \
    size_t gc_hdr = gc_ss_hdr(s);                                              \
    (s).h = malloc(gc_hdr + gc_stride * (init_cap));                           \
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
#define gc_ss_ensure(s)                                                        \
  do {                                                                         \
    SSetHeader *gc_ensure_header = (s).h;                                      \
    if (gc_ensure_header->len >= gc_ensure_header->cap) {                      \
      size_t gc_ensure_capacity =                                              \
          gc_ensure_header->cap ? gc_ensure_header->cap * 2 : 1;               \
      size_t gc_ensure_stride = gc_ss_stride(s);                               \
      size_t gc_ensure_offset = gc_ss_hdr(s);                                  \
      gc_ensure_header =                                                       \
          realloc(gc_ensure_header,                                            \
                  gc_ensure_offset + gc_ensure_stride * gc_ensure_capacity);   \
      assert(gc_ensure_header);                                                \
      gc_ensure_header->cap = gc_ensure_capacity;                              \
      (s).h = gc_ensure_header;                                                \
    }                                                                          \
  } while (0)

// —— insert element (no‐op if already present) ——
#define bset_add(s, key_expr)                                                  \
  do {                                                                         \
    if (!(s).h)                                                                \
      bset_init(s, 1);                                                         \
    typeof((s)._key) gc_k = (key_expr);                                        \
    SSetHeader *gc_sh = (s).h;                                                 \
    size_t gc_n = gc_sh->len, gc_lo = 0, gc_hi = gc_n;                         \
    bool gc_found = false;                                                     \
                                                                               \
    /* binary search for insertion point */                                    \
    while (gc_lo < gc_hi) {                                                    \
      size_t gc_mid = (gc_lo + gc_hi) >> 1;                                    \
      int gc_c = gc_ss_cmp(s, gc_sh, gc_k, gc_mid);                            \
      if (gc_c < 0)                                                            \
        gc_hi = gc_mid;                                                        \
      else if (gc_c > 0)                                                       \
        gc_lo = gc_mid + 1;                                                    \
      else {                                                                   \
        gc_found = true;                                                       \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    if (!gc_found) {                                                           \
      gc_ss_ensure(s);                                                         \
      gc_sh = (s).h;                                                           \
      gc_n = gc_sh->len;                                                       \
      /* shift right to make room at index gc_lo */                            \
      size_t gc_stride = gc_ss_stride(s);                                      \
      char *gc_base = gc_ss_data(s);                                           \
      memmove(gc_base + (gc_lo + 1) * gc_stride,                               \
              gc_base + (gc_lo) * gc_stride, (gc_n - gc_lo) * gc_stride);      \
      /* insert new element */                                                 \
      *gc_ss_keyptr(s, gc_lo) = gc_k;                                          \
      gc_sh->len = gc_n + 1;                                                   \
    }                                                                          \
  } while (0)

// —— test membership ——
#define bset_contains(s, key_expr)                                             \
  ({                                                                           \
    bool gc_hit = false;                                                       \
    if ((s).h) {                                                               \
      typeof((s)._key) gc_k = (key_expr);                                      \
      SSetHeader *gc_sh = (s).h;                                               \
      size_t gc_lo = 0, gc_hi = gc_sh->len;                                    \
      while (gc_lo < gc_hi) {                                                  \
        size_t gc_mid = (gc_lo + gc_hi) >> 1;                                  \
        int gc_c = gc_ss_cmp(s, gc_sh, gc_k, gc_mid);                          \
        if (gc_c < 0)                                                          \
          gc_hi = gc_mid;                                                      \
        else if (gc_c > 0)                                                     \
          gc_lo = gc_mid + 1;                                                  \
        else {                                                                 \
          gc_hit = true;                                                       \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    gc_hit;                                                                    \
  })

// —— remove an element: returns 1 if it was present, 0 otherwise ——
#define bset_remove(s, key_expr)                                               \
  ({                                                                           \
    int gc_removed = 0;                                                        \
    if ((s).h) {                                                               \
      typeof((s)._key) gc_k = (key_expr);                                      \
      SSetHeader *gc_sh = (s).h;                                               \
      size_t gc_lo = 0, gc_hi = gc_sh->len;                                    \
      while (gc_lo < gc_hi) {                                                  \
        size_t gc_mid = (gc_lo + gc_hi) >> 1;                                  \
        int gc_c = gc_ss_cmp(s, gc_sh, gc_k, gc_mid);                          \
        if (gc_c < 0)                                                          \
          gc_hi = gc_mid;                                                      \
        else if (gc_c > 0)                                                     \
          gc_lo = gc_mid + 1;                                                  \
        else {                                                                 \
          size_t gc_stride = gc_ss_stride(s);                                  \
          char *gc_base = gc_ss_data(s);                                       \
          memmove(gc_base + gc_mid * gc_stride,                                \
                  gc_base + (gc_mid + 1) * gc_stride,                          \
                  (gc_sh->len - gc_mid - 1) * gc_stride);                      \
          gc_sh->len--;                                                        \
          gc_removed = 1;                                                      \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    gc_removed;                                                                \
  })

// —— iterate in sorted order ——
#define bset_for(s, it)                                                        \
  for (size_t gc_i = 0, gc_n = ss_len(s); gc_i < gc_n; ++gc_i)                 \
    for (int gc_once = 1; gc_once; gc_once = 0)                                \
      for (typeof((s)._key) it = *gc_ss_keyptr(s, gc_i); gc_once; gc_once = 0)

#endif // BTREESET_H
