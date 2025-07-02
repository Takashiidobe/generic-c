#ifndef SORTED_MAP_H
#define SORTED_MAP_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// ─── Internal header for dynamic storage ───────────────────────────────────
typedef struct SMapHeader {
  size_t len, cap;
  char data[]; // flexible-array of Pair<K,V>
} SMapHeader;
#define _sm_header(m)                                                          \
  ((SMapHeader *)(((char *)(m)) - offsetof(SMapHeader, data)))

// ─── Pair type and map alias ──────────────────────────────────────────────
#define Pair(K, V)                                                             \
  struct {                                                                     \
    K key;                                                                     \
    V val;                                                                     \
  }
#define SortedMap(K, V) Pair(K, V) *

// ─── length, capacity, and free ──────────────────────────────────────────
#define sm_len(m) ((m) ? _sm_header(m)->len : 0)
#define sm_cap(m) ((m) ? _sm_header(m)->cap : 0)
#define sm_free(m)                                                             \
  do {                                                                         \
    if (m) {                                                                   \
      free(_sm_header(m));                                                     \
      (m) = NULL;                                                              \
    }                                                                          \
  } while (0)

// ─── Internal grow routine ───────────────────────────────────────────────
static inline void _sm_grow(void **arr, size_t needed, size_t elem_size) {
  SMapHeader *h = *arr ? _sm_header(*arr) : NULL;
  size_t old_len = h ? h->len : 0;
  size_t new_len = old_len + needed;
  size_t cap = h ? h->cap : 0;

  if (new_len > cap) {
    size_t dbl = cap ? cap * 2 : 1;
    size_t new_cap = dbl > new_len ? dbl : new_len;
    size_t hdr_sz = offsetof(SMapHeader, data);

    if (h) {
      h = realloc(h, hdr_sz + new_cap * elem_size);
      assert(h);
    } else {
      h = malloc(hdr_sz + new_cap * elem_size);
      assert(h);
      h->len = 0;
    }
    h->cap = new_cap;
  }
  h->len = new_len;
  *arr = h->data;
}

// ─── insert or update, keeping keys sorted ────────────────────────────────
#define sorted_map_put(m, K, V, KEY, VAL)                                      \
  do {                                                                         \
    __typeof__(*(m)) _p = {.key = (KEY), .val = (VAL)};                        \
    size_t _n = sm_len(m), _lo = 0, _hi = _n;                                  \
    bool _replaced = false;                                                    \
    /* binary search */                                                        \
    while (_lo < _hi) {                                                        \
      size_t _mid = (_lo + _hi) / 2;                                           \
      if (_p.key < (m)[_mid].key)                                              \
        _hi = _mid;                                                            \
      else if (_p.key > (m)[_mid].key)                                         \
        _lo = _mid + 1;                                                        \
      else {                                                                   \
        (m)[_mid].val = _p.val;                                                \
        _replaced = true;                                                      \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (!_replaced) {                                                          \
      _sm_grow((void **)&(m), 1, sizeof *(m));                                 \
      memmove(&(m)[_lo + 1], &(m)[_lo], (_n - _lo) * sizeof *(m));             \
      (m)[_lo] = _p;                                                           \
    }                                                                          \
  } while (0)

// ─── lookup: returns V* or NULL ─────────────────────────────────────────
#define sorted_map_get(m, K, V, KEY)                                           \
  ({                                                                           \
    size_t _n = sm_len(m), _lo = 0, _hi = _n;                                  \
    V *_res = NULL;                                                            \
    while (_lo < _hi) {                                                        \
      size_t _mid = (_lo + _hi) / 2;                                           \
      if ((KEY) < (m)[_mid].key)                                               \
        _hi = _mid;                                                            \
      else if ((KEY) > (m)[_mid].key)                                          \
        _lo = _mid + 1;                                                        \
      else {                                                                   \
        _res = &(m)[_mid].val;                                                 \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    _res;                                                                      \
  })

// ─── iterate in sorted order ────────────────────────────────────────────
#define sorted_map_for(it, m)                                                  \
  for (size_t _i = 0, _n = sm_len(m); _i < _n; ++_i)                           \
    for (__typeof__(*(m)) *it = &((m)[_i]); it; it = NULL)

#endif // SORTED_MAP_H
