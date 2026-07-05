// bmap.h
#ifndef btreemap_h
#define btreemap_h

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../hash/hash.h"

// —— internal header: length, capacity, comparator, and raw data array ——
// cmp == NULL means order keys with the built-in < / > operators (scalars);
// set a comparator (e.g. gc_str_cmp) via bmap_init_with for other key types.
typedef struct {
  size_t len, cap;
  gc_cmp_fn cmp;
  max_align_t data[];
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
#define gc_sm_align_up(n, a) (((n) + (a) - 1) & ~((size_t)(a) - 1))
// larger of the key/value alignments — the alignment of a whole slot
#define gc_sm_align(m)                                                           \
  (_Alignof(typeof((m)._key)) > _Alignof(typeof((m)._val))                     \
       ? _Alignof(typeof((m)._key))                                            \
       : _Alignof(typeof((m)._val)))
// byte offset of the value within a slot: key size rounded up to val alignment
#define gc_sm_valoff(m) gc_sm_align_up(sizeof((m)._key), _Alignof(typeof((m)._val)))
// storage stride: value end rounded up to slot alignment, so every slot and
// both of its fields stay aligned
#define gc_sm_stride(m) gc_sm_align_up(gc_sm_valoff(m) + sizeof((m)._val), gc_sm_align(m))
// raw byte pointer
#define gc_sm_data(m) ((char *)(m).h->data)
// pointer to key slot i
#define gc_sm_keyptr(m, i)                                                       \
  ((typeof((m)._key) *)(void *)(gc_sm_data(m) + (i) * gc_sm_stride(m)))
// pointer to val slot i
#define gc_sm_valptr(m, i)                                                       \
  ((typeof((m)._val) *)(void *)(gc_sm_data(m) + (i) * gc_sm_stride(m) +        \
                                gc_sm_valoff(m)))
// three-way compare of key gc_k against slot i, honoring a custom comparator
#define gc_sm_cmp(m, sh, k, i)                                                   \
  ((sh)->cmp ? (sh)->cmp(&(k), gc_sm_keyptr(m, i), sizeof((m)._key))             \
             : ((k) < *gc_sm_keyptr(m, i) ? -1                                   \
                                        : ((k) > *gc_sm_keyptr(m, i) ? 1 : 0)))

// —— initialize with a custom comparator (use gc_str_cmp for string keys) ——
#define bmap_init_with(m, init_cap, cmp_fn)                                    \
  do {                                                                         \
    static_assert(gc_sm_align(m) <= _Alignof(max_align_t),                      \
                  "bmap entry alignment exceeds max_align_t");                 \
    size_t gc_stride = gc_sm_stride(m);                                           \
    size_t gc_hdr = offsetof(SMapHeader, data);                                 \
    (m).h = malloc(gc_hdr + gc_stride * (init_cap));                             \
    assert((m).h);                                                             \
    (m).h->len = 0;                                                            \
    (m).h->cap = (init_cap);                                                   \
    (m).h->cmp = (cmp_fn);                                                     \
  } while (0)

// —— initialize & free (built-in scalar ordering) ——
#define bmap_init(m, init_cap) bmap_init_with(m, init_cap, NULL)

#define bmap_free(m)                                                           \
  do {                                                                         \
    if ((m).h) {                                                               \
      free((m).h);                                                             \
      (m).h = NULL;                                                            \
    }                                                                          \
  } while (0)

// —— grow capacity when full ——
#define gc_sm_ensure(m)                                                          \
  do {                                                                         \
    SMapHeader *gc_ensure_header = (m).h;                                       \
    if (gc_ensure_header->len >= gc_ensure_header->cap) {                       \
      size_t gc_ensure_capacity =                                               \
          gc_ensure_header->cap ? gc_ensure_header->cap * 2 : 1;               \
      size_t gc_ensure_stride = gc_sm_stride(m);                                \
      size_t gc_ensure_offset = offsetof(SMapHeader, data);                     \
      gc_ensure_header =                                                        \
          realloc(gc_ensure_header,                                             \
                  gc_ensure_offset + gc_ensure_stride * gc_ensure_capacity);    \
      assert(gc_ensure_header);                                                 \
      gc_ensure_header->cap = gc_ensure_capacity;                               \
      (m).h = gc_ensure_header;                                                 \
    }                                                                          \
  } while (0)

// —— insert or overwrite ——
#define bmap_put(m, key_expr, val_expr)                                        \
  do {                                                                         \
    if (!(m).h)                                                                \
      bmap_init(m, 1);                                                         \
    typeof((m)._key) gc_k = (key_expr);                                         \
    typeof((m)._val) gc_v = (val_expr);                                         \
    SMapHeader *gc_sh = (m).h;                                                  \
    size_t gc_n = gc_sh->len, gc_lo = 0, gc_hi = gc_n;                              \
    bool gc_found = false;                                                      \
                                                                               \
    /* binary search */                                                        \
    while (gc_lo < gc_hi) {                                                      \
      size_t gc_mid = (gc_lo + gc_hi) >> 1;                                       \
      int gc_c = gc_sm_cmp(m, gc_sh, gc_k, gc_mid);                                  \
      if (gc_c < 0)                                                             \
        gc_hi = gc_mid;                                                          \
      else if (gc_c > 0)                                                        \
        gc_lo = gc_mid + 1;                                                      \
      else {                                                                   \
        *gc_sm_valptr(m, gc_mid) = gc_v;                                           \
        gc_found = true;                                                        \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    if (!gc_found) {                                                            \
      gc_sm_ensure(m);                                                           \
      gc_sh = (m).h;                                                            \
      gc_n = gc_sh->len;                                                         \
      size_t gc_stride = gc_sm_stride(m);                                         \
      char *gc_base = gc_sm_data(m);                                              \
                                                                               \
      /* shift right to make room */                                           \
      memmove(gc_base + (gc_lo + 1) * gc_stride, gc_base + gc_lo * gc_stride,        \
              (gc_n - gc_lo) * gc_stride);                                        \
                                                                               \
      *gc_sm_keyptr(m, gc_lo) = gc_k;                                              \
      *gc_sm_valptr(m, gc_lo) = gc_v;                                              \
      gc_sh->len = gc_n + 1;                                                     \
    }                                                                          \
  } while (0)

// —— lookup: returns pointer to V or NULL ——
#define bmap_get(m, key_expr)                                                  \
  ({                                                                           \
    typeof((m)._val) *gc_res = NULL;                                            \
    if ((m).h) {                                                               \
      typeof((m)._key) gc_k = (key_expr);                                       \
      SMapHeader *gc_sh = (m).h;                                                \
      size_t gc_lo = 0, gc_hi = gc_sh->len;                                       \
      while (gc_lo < gc_hi) {                                                    \
        size_t gc_mid = (gc_lo + gc_hi) >> 1;                                     \
        int gc_c = gc_sm_cmp(m, gc_sh, gc_k, gc_mid);                                \
        if (gc_c < 0)                                                           \
          gc_hi = gc_mid;                                                        \
        else if (gc_c > 0)                                                      \
          gc_lo = gc_mid + 1;                                                    \
        else {                                                                 \
          gc_res = gc_sm_valptr(m, gc_mid);                                        \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    gc_res;                                                                     \
  })

// —— remove a key: returns 1 if it was present, 0 otherwise ——
#define bmap_remove(m, key_expr)                                               \
  ({                                                                           \
    int gc_removed = 0;                                                         \
    if ((m).h) {                                                               \
      typeof((m)._key) gc_k = (key_expr);                                       \
      SMapHeader *gc_sh = (m).h;                                                \
      size_t gc_lo = 0, gc_hi = gc_sh->len;                                       \
      while (gc_lo < gc_hi) {                                                    \
        size_t gc_mid = (gc_lo + gc_hi) >> 1;                                     \
        int gc_c = gc_sm_cmp(m, gc_sh, gc_k, gc_mid);                                \
        if (gc_c < 0)                                                           \
          gc_hi = gc_mid;                                                        \
        else if (gc_c > 0)                                                      \
          gc_lo = gc_mid + 1;                                                    \
        else {                                                                 \
          size_t gc_stride = gc_sm_stride(m);                                     \
          char *gc_base = gc_sm_data(m);                                          \
          memmove(gc_base + gc_mid * gc_stride,                                   \
                  gc_base + (gc_mid + 1) * gc_stride,                             \
                  (gc_sh->len - gc_mid - 1) * gc_stride);                         \
          gc_sh->len--;                                                         \
          gc_removed = 1;                                                       \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    gc_removed;                                                                 \
  })

// —— iterate in sorted order ——
#define bmap_for(m, it_k, it_v)                                                \
  for (size_t gc_i = 0, gc_n = sm_len(m); gc_i < gc_n; ++gc_i)                      \
    for (int gc_once1 = 1; gc_once1; gc_once1 = 0)                                \
      for (typeof((m)._key) it_k = *gc_sm_keyptr(m, gc_i); gc_once1; gc_once1 = 0)  \
        for (int gc_once2 = 1; gc_once2; gc_once2 = 0)                            \
          for (typeof((m)._val) it_v = *gc_sm_valptr(m, gc_i); gc_once2;           \
               gc_once2 = 0)

#endif // btreemap_h
