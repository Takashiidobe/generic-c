#ifndef HASHMAP_H
#define HASHMAP_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../hash/hash.h"

// —— internal node type (chaining) ——
typedef struct HNode {
  struct HNode *next;
  char data[]; // key bytes followed by value bytes
} HNode;

// —— core map metadata ——
typedef struct {
  size_t key_size, val_size, bucket_count;
  HNode **buckets;
} HashMap;

// —— declare a map variable with key type K and value type V ——
#define Map(K, V)                                                              \
  struct {                                                                     \
    HashMap *h;                                                                \
    K _key;                                                                    \
    V _val;                                                                    \
  }

// —— initialize a map with `n_buckets` ——
#define map_init(m, n_buckets)                                                 \
  do {                                                                         \
    (m).h = malloc(sizeof(HashMap));                                           \
    assert((m).h);                                                             \
    (m).h->key_size = sizeof((m)._key);                                        \
    (m).h->val_size = sizeof((m)._val);                                        \
    (m).h->bucket_count = (n_buckets);                                         \
    (m).h->buckets = calloc((m).h->bucket_count, sizeof(HNode *));             \
    assert((m).h->buckets);                                                    \
  } while (0)

// —— free all nodes and buckets ——
#define map_free(m)                                                            \
  do {                                                                         \
    HashMap *_ht = (m).h;                                                      \
    for (size_t _i = 0; _i < _ht->bucket_count; ++_i) {                        \
      HNode *_n = _ht->buckets[_i];                                            \
      while (_n) {                                                             \
        HNode *_nx = _n->next;                                                 \
        free(_n);                                                              \
        _n = _nx;                                                              \
      }                                                                        \
    }                                                                          \
    free(_ht->buckets);                                                        \
    free(_ht);                                                                 \
    (m).h = NULL;                                                              \
  } while (0)

// —— insert or overwrite a (key, val) pair ——
#define map_put(m, key_expr, val_expr)                                         \
  do {                                                                         \
    HashMap *_ht = (m).h;                                                      \
    typeof((m)._key) _key = (key_expr);                                        \
    typeof((m)._val) _val = (val_expr);                                        \
    size_t _b = _hash_bytes(&_key, _ht->key_size) % _ht->bucket_count;         \
    HNode **_buck = &_ht->buckets[_b];                                         \
    HNode *_n;                                                                 \
    int _found = 0;                                                            \
    for (_n = *_buck; _n; _n = _n->next) {                                     \
      if (memcmp(_n->data, &_key, _ht->key_size) == 0) {                       \
        memcpy(_n->data + _ht->key_size, &_val, _ht->val_size);                \
        _found = 1;                                                            \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (!_found) {                                                             \
      HNode *_new = malloc(sizeof(HNode) + _ht->key_size + _ht->val_size);     \
      assert(_new);                                                            \
      memcpy(_new->data, &_key, _ht->key_size);                                \
      memcpy(_new->data + _ht->key_size, &_val, _ht->val_size);                \
      _new->next = *_buck;                                                     \
      *_buck = _new;                                                           \
    }                                                                          \
  } while (0)

// —— lookup: returns pointer to value or NULL ——
#define map_get(m, key_expr)                                                   \
  ({                                                                           \
    HashMap *_ht = (m).h;                                                      \
    typeof((m)._key) _key = (key_expr);                                        \
    size_t _b = _hash_bytes(&_key, _ht->key_size) % _ht->bucket_count;         \
    HNode *_n = _ht->buckets[_b];                                              \
    typeof((m)._val) *_out = NULL;                                             \
    while (_n) {                                                               \
      if (memcmp(_n->data, &_key, _ht->key_size) == 0) {                       \
        _out = (typeof((m)._val) *)(_n->data + _ht->key_size);                 \
        break;                                                                 \
      }                                                                        \
      _n = _n->next;                                                           \
    }                                                                          \
    _out;                                                                      \
  })

// —— iterate all (key, value) pairs ——
#define map_for(m, it_k, it_v)                                                 \
  for (size_t _bi = 0; _bi < (m).h->bucket_count; ++_bi)                       \
    for (HNode *_n = (m).h->buckets[_bi]; _n; _n = _n->next)                   \
      for (int _once_k = 1; _once_k; _once_k = 0)                              \
        for (typeof((m)._key) it_k = *(typeof((m)._key) *)_n->data; _once_k;   \
             _once_k = 0)                                                      \
          for (int _once_v = 1; _once_v; _once_v = 0)                          \
            for (typeof((m)._val) it_v =                                       \
                     *(typeof((m)._val) *)(_n->data + (m).h->key_size);        \
                 _once_v; _once_v = 0)

// —— iterate keys only ——
#define map_keys(m, it_k)                                                      \
  for (size_t _bi = 0; _bi < (m).h->bucket_count; ++_bi)                       \
    for (HNode *_n = (m).h->buckets[_bi]; _n; _n = _n->next)                   \
      for (int _once_k = 1; _once_k; _once_k = 0)                              \
        for (typeof((m)._key) it_k = *(typeof((m)._key) *)_n->data; _once_k;   \
             _once_k = 0)

// —— iterate values only ——
#define map_vals(m, it_v)                                                      \
  for (size_t _bi = 0; _bi < (m).h->bucket_count; ++_bi)                       \
    for (HNode *_n = (m).h->buckets[_bi]; _n; _n = _n->next)                   \
      for (int _once_v = 1; _once_v; _once_v = 0)                              \
        for (typeof((m)._val) it_v =                                           \
                 *(typeof((m)._val) *)(_n->data + (m).h->key_size);            \
             _once_v; _once_v = 0)

#endif // HASHMAP_H
