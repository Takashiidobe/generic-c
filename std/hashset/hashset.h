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
  char data[]; // key bytes
} SNode;

// —— core set metadata ——
typedef struct {
  size_t key_size;
  size_t bucket_count;
  SNode **buckets;
} HashSet;

// —— declare a set variable with element type E ——
#define Set(E)                                                                 \
  struct {                                                                     \
    HashSet *h;                                                                \
    E _key;                                                                    \
  }

// —— initialize a set with `n_buckets` ——
#define set_init(s, n_buckets)                                                 \
  do {                                                                         \
    (s).h = malloc(sizeof(HashSet));                                           \
    assert((s).h);                                                             \
    (s).h->key_size = sizeof((s)._key);                                        \
    (s).h->bucket_count = (n_buckets);                                         \
    (s).h->buckets = calloc((s).h->bucket_count, sizeof(SNode *));             \
    assert((s).h->buckets);                                                    \
  } while (0)

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
    HashSet *_hs = (s).h;                                                      \
    typeof((s)._key) _key = (key_expr);                                        \
    size_t _b = _hash_bytes(&_key, _hs->key_size) % _hs->bucket_count;         \
    SNode **_buck = &_hs->buckets[_b];                                         \
    SNode *_n;                                                                 \
    int _found = 0;                                                            \
    for (_n = *_buck; _n; _n = _n->next) {                                     \
      if (memcmp(_n->data, &_key, _hs->key_size) == 0) {                       \
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
    }                                                                          \
  } while (0)

// —— test membership: returns 1 if present, 0 otherwise ——
#define set_contains(s, key_expr)                                              \
  ({                                                                           \
    HashSet *_hs = (s).h;                                                      \
    typeof((s)._key) _key = (key_expr);                                        \
    size_t _b = _hash_bytes(&_key, _hs->key_size) % _hs->bucket_count;         \
    SNode *_n = _hs->buckets[_b];                                              \
    int _hit = 0;                                                              \
    while (_n) {                                                               \
      if (memcmp(_n->data, &_key, _hs->key_size) == 0) {                       \
        _hit = 1;                                                              \
        break;                                                                 \
      }                                                                        \
      _n = _n->next;                                                           \
    }                                                                          \
    _hit;                                                                      \
  })

// —— iterate all elements ——
#define set_for(s, it)                                                         \
  for (size_t _bi = 0; _bi < (s).h->bucket_count; ++_bi)                       \
    for (SNode *_n = (s).h->buckets[_bi]; _n; _n = _n->next)                   \
      for (int _once = 1; _once; _once = 0)                                    \
        for (typeof((s)._key) it = *(typeof((s)._key) *)_n->data; _once;       \
             _once = 0)

#endif // HASHSET_H
