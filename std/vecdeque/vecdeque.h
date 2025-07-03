#ifndef DEQUE_H
#define DEQUE_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// ─── Internal header for circular deque storage ───────────────────────────
typedef struct DQHeader {
  size_t len, cap, front;
  char data[]; // flexible-array of T
} DQHeader;

#define _dq_header(d) ((DQHeader *)(((char *)(d)) - offsetof(DQHeader, data)))

// ─── Deque type alias ────────────────────────────────────────────────────
#define Deque(T) T *

// ─── length, capacity, and free ─────────────────────────────────────────
#define dq_len(d) ((d) ? _dq_header(d)->len : 0)
#define dq_cap(d) ((d) ? _dq_header(d)->cap : 0)
#define dq_free(d)                                                             \
  do {                                                                         \
    if (d) {                                                                   \
      free(_dq_header(d));                                                     \
      (d) = NULL;                                                              \
    }                                                                          \
  } while (0)

// ─── internal grow: allocate larger buffer and realign ────────────────────
static inline void _dq_grow(void **arr, size_t elem_size) {
  DQHeader *h = *arr ? _dq_header(*arr) : NULL;
  size_t old_cap = h ? h->cap : 0;
  size_t new_cap = old_cap ? old_cap * 2 : 1;
  size_t hdr_sz = offsetof(DQHeader, data);

  // allocate new header + data
  DQHeader *nh = malloc(hdr_sz + new_cap * elem_size);
  assert(nh);
  nh->len = h ? h->len : 0;
  nh->cap = new_cap;
  nh->front = 0;

  // copy existing elements in order
  if (h && nh->len > 0) {
    size_t c = h->cap;
    char *old_data = (char *)h->data;
    char *new_data = (char *)nh->data;
    for (size_t i = 0; i < nh->len; ++i) {
      memcpy(new_data + i * elem_size,
             old_data + ((h->front + i) % c) * elem_size, elem_size);
    }
  }

  // free old and set new
  if (h)
    free(h);
  *arr = nh->data;
}

// ─── push to back ────────────────────────────────────────────────────────
#define dq_push_back(d, VAL)                                                   \
  do {                                                                         \
    __typeof__(*(d)) _v = (VAL);                                               \
    size_t _len = dq_len(d);                                                   \
    if (_len == dq_cap(d))                                                     \
      _dq_grow((void **)&(d), sizeof *(d));                                    \
    DQHeader *hdr = _dq_header(d);                                             \
    size_t idx = (hdr->front + hdr->len) % hdr->cap;                           \
    d[idx] = _v;                                                               \
    hdr->len++;                                                                \
  } while (0)

// ─── push to front ───────────────────────────────────────────────────────
#define dq_push_front(d, VAL)                                                  \
  do {                                                                         \
    __typeof__(*(d)) _v = (VAL);                                               \
    size_t _len = dq_len(d);                                                   \
    if (_len == dq_cap(d))                                                     \
      _dq_grow((void **)&(d), sizeof *(d));                                    \
    DQHeader *hdr = _dq_header(d);                                             \
    hdr->front = hdr->cap ? (hdr->front + hdr->cap - 1) % hdr->cap : 0;        \
    d[hdr->front] = _v;                                                        \
    hdr->len++;                                                                \
  } while (0)

// ─── pop from back (returns element) ──────────────────────────────────────
#define dq_pop_back(d)                                                         \
  ({                                                                           \
    DQHeader *hdr = _dq_header(d);                                             \
    assert(hdr->len > 0);                                                      \
    size_t idx = (hdr->front + hdr->len - 1) % hdr->cap;                       \
    __typeof__(*(d)) _res = d[idx];                                            \
    hdr->len--;                                                                \
    _res;                                                                      \
  })

// ─── pop from front (returns element) ─────────────────────────────────────
#define dq_pop_front(d)                                                        \
  ({                                                                           \
    DQHeader *hdr = _dq_header(d);                                             \
    assert(hdr->len > 0);                                                      \
    __typeof__(*(d)) _res = d[hdr->front];                                     \
    hdr->front = (hdr->front + 1) % hdr->cap;                                  \
    hdr->len--;                                                                \
    _res;                                                                      \
  })

// ─── random-access get ────────────────────────────────────────────────────
#define dq_get(d, IDX) ((d)[(((IDX) + _dq_header(d)->front) % dq_cap(d))])

// ─── iterate in deque order ───────────────────────────────────────────────
#define deque_for(it, d)                                                       \
  for (size_t _i = 0, _n = dq_len(d); _i < _n; ++_i)                           \
    for (__typeof__(*(d)) *it = &dq_get(d, _i); it; it = NULL)

#endif // DEQUE_H
