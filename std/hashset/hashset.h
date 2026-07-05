#ifndef HASHSET_H
#define HASHSET_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../hash/hash.h"

// —— internal node type (chaining) ——
typedef struct SNode {
  struct SNode *next;
  max_align_t data[];
} SNode;

// —— core set metadata ——
typedef struct {
  size_t key_size;
  size_t bucket_count;
  size_t count;
  gc_hash_fn hash;
  gc_eq_fn eq;
  SNode **buckets;
} HashSet;

// —— declare a set variable with element type E ——
#define Set(E)                                                                 \
  struct {                                                                     \
    HashSet *h;                                                                \
    E _key;                                                                    \
  }

// —— initialize with custom hash + equality ——
// use gc_str_hash / gc_str_eq for `const char *` elements.
#define set_init_with(s, n_buckets, hash_fn, eq_fn)                            \
  do {                                                                         \
    static_assert(_Alignof(typeof((s)._key)) <= _Alignof(max_align_t),         \
                  "set element alignment exceeds max_align_t");                \
    (s).h = malloc(sizeof(HashSet));                                           \
    assert((s).h);                                                             \
    (s).h->key_size = sizeof((s)._key);                                        \
    (s).h->bucket_count = (n_buckets);                                         \
    (s).h->count = 0;                                                          \
    (s).h->hash = (hash_fn);                                                   \
    (s).h->eq = (eq_fn);                                                       \
    (s).h->buckets = calloc((s).h->bucket_count, sizeof(SNode *));             \
    assert((s).h->buckets);                                                    \
  } while (0)

// —— initialize a set with `n_buckets` (raw-byte hashing; scalar elements) ——
#define set_init(s, n_buckets)                                                 \
  set_init_with(s, n_buckets, gc_bytes_hash, gc_bytes_eq)

// —— number of live elements ——
#define set_len(s) ((s).h ? (s).h->count : 0)

// —— grow + rehash once the load factor passes 0.75 ——
static inline void gc_hashset_maybe_grow(HashSet *h) {
  if (h->count * 4 < h->bucket_count * 3)
    return;
  size_t nc = h->bucket_count ? h->bucket_count * 2 : 1;
  SNode **nb = calloc(nc, sizeof(SNode *));
  assert(nb);
  for (size_t i = 0; i < h->bucket_count; ++i) {
    SNode *n = h->buckets[i];
    while (n) {
      SNode *next = n->next;
      size_t b = h->hash(n->data, h->key_size) % nc;
      n->next = nb[b];
      nb[b] = n;
      n = next;
    }
  }
  free(h->buckets);
  h->buckets = nb;
  h->bucket_count = nc;
}

// —— free all nodes and buckets ——
#define set_free(s)                                                            \
  do {                                                                         \
    HashSet *_hs = (s).h;                                                      \
    for (size_t _i = 0; _i < _hs->bucket_count; ++_i) {                        \
      SNode *_n = _hs->buckets[_i];                                            \
      while (_n) {                                                             \
        SNode *_nx = _n->next;                                                 \
        free(_n);                                                              \
        _n = _nx;                                                              \
      }                                                                        \
    }                                                                          \
    free(_hs->buckets);                                                        \
    free(_hs);                                                                 \
    (s).h = NULL;                                                              \
  } while (0)

// —— add an element to the set (no-op if already present) ——
#define set_add(s, key_expr)                                                   \
  do {                                                                         \
    gc_hashset_maybe_grow((s).h);                                              \
    HashSet *_hs = (s).h;                                                      \
    typeof((s)._key) _key = (key_expr);                                        \
    size_t _b = _hs->hash(&_key, _hs->key_size) % _hs->bucket_count;           \
    SNode **_buck = &_hs->buckets[_b];                                         \
    SNode *_n;                                                                 \
    int _found = 0;                                                            \
    for (_n = *_buck; _n; _n = _n->next) {                                     \
      if (_hs->eq(_n->data, &_key, _hs->key_size)) {                           \
        _found = 1;                                                            \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (!_found) {                                                             \
      SNode *_new = malloc(sizeof(SNode) + _hs->key_size);                     \
      assert(_new);                                                            \
      memcpy(_new->data, &_key, _hs->key_size);                                \
      _new->next = *_buck;                                                     \
      *_buck = _new;                                                           \
      _hs->count++;                                                            \
    }                                                                          \
  } while (0)

// —— test membership: returns 1 if present, 0 otherwise ——
#define set_contains(s, key_expr)                                              \
  ({                                                                           \
    HashSet *_hs = (s).h;                                                      \
    typeof((s)._key) _key = (key_expr);                                        \
    size_t _b = _hs->hash(&_key, _hs->key_size) % _hs->bucket_count;           \
    SNode *_n = _hs->buckets[_b];                                              \
    int _hit = 0;                                                              \
    while (_n) {                                                               \
      if (_hs->eq(_n->data, &_key, _hs->key_size)) {                           \
        _hit = 1;                                                              \
        break;                                                                 \
      }                                                                        \
      _n = _n->next;                                                           \
    }                                                                          \
    _hit;                                                                      \
  })

// —— remove an element: returns 1 if it was present, 0 otherwise ——
#define set_remove(s, key_expr)                                                \
  ({                                                                           \
    HashSet *_hs = (s).h;                                                      \
    typeof((s)._key) _key = (key_expr);                                        \
    size_t _b = _hs->hash(&_key, _hs->key_size) % _hs->bucket_count;           \
    SNode **_pp = &_hs->buckets[_b];                                           \
    int _removed = 0;                                                          \
    while (*_pp) {                                                             \
      SNode *_n = *_pp;                                                        \
      if (_hs->eq(_n->data, &_key, _hs->key_size)) {                           \
        *_pp = _n->next;                                                       \
        free(_n);                                                              \
        _hs->count--;                                                          \
        _removed = 1;                                                          \
        break;                                                                 \
      }                                                                        \
      _pp = &_n->next;                                                         \
    }                                                                          \
    _removed;                                                                  \
  })

// —— iterate all elements ——
#define set_for(s, it)                                                         \
  for (size_t _bi = 0; _bi < (s).h->bucket_count; ++_bi)                       \
    for (SNode *_n = (s).h->buckets[_bi]; _n; _n = _n->next)                   \
      for (int _once = 1; _once; _once = 0)                                    \
        for (typeof((s)._key) it = *(typeof((s)._key) *)_n->data; _once;       \
             _once = 0)

#endif // HASHSET_H
