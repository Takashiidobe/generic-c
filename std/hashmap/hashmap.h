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
// val_off = key_size rounded up to the value's alignment, so the value slot in
// each node is properly aligned for its type. count tracks live entries.
typedef struct {
  size_t key_size, val_size, val_off, bucket_count, count;
  gc_hash_fn hash;
  gc_eq_fn eq;
  HNode **buckets;
} HashMap;

#define _hm_align_up(n, a) (((n) + (a) - 1) & ~((size_t)(a) - 1))

// —— declare a map variable with key type K and value type V ——
#define Map(K, V)                                                              \
  struct {                                                                     \
    HashMap *h;                                                                \
    K _key;                                                                    \
    V _val;                                                                    \
  }

// —— initialize with custom hash + equality ——
// use gc_str_hash / gc_str_eq for `const char *` keys.
#define map_init_with(m, n_buckets, hash_fn, eq_fn)                            \
  do {                                                                         \
    (m).h = malloc(sizeof(HashMap));                                           \
    assert((m).h);                                                             \
    (m).h->key_size = sizeof((m)._key);                                        \
    (m).h->val_size = sizeof((m)._val);                                        \
    (m).h->val_off =                                                           \
        _hm_align_up(sizeof((m)._key), _Alignof(typeof((m)._val)));            \
    (m).h->bucket_count = (n_buckets);                                         \
    (m).h->count = 0;                                                          \
    (m).h->hash = (hash_fn);                                                   \
    (m).h->eq = (eq_fn);                                                       \
    (m).h->buckets = calloc((m).h->bucket_count, sizeof(HNode *));             \
    assert((m).h->buckets);                                                    \
  } while (0)

// —— initialize a map with `n_buckets` (raw-byte hashing; scalar keys) ——
#define map_init(m, n_buckets)                                                 \
  map_init_with(m, n_buckets, gc_bytes_hash, gc_bytes_eq)

// —— number of live entries ——
#define map_len(m) ((m).h ? (m).h->count : 0)

// —— grow + rehash once the load factor passes 0.75 ——
static inline void _hashmap_maybe_grow(HashMap *h) {
  if (h->count * 4 < h->bucket_count * 3)
    return;
  size_t nc = h->bucket_count ? h->bucket_count * 2 : 1;
  HNode **nb = calloc(nc, sizeof(HNode *));
  assert(nb);
  for (size_t i = 0; i < h->bucket_count; ++i) {
    HNode *n = h->buckets[i];
    while (n) {
      HNode *next = n->next;
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
    _hashmap_maybe_grow((m).h);                                                \
    HashMap *_ht = (m).h;                                                      \
    typeof((m)._key) _key = (key_expr);                                        \
    typeof((m)._val) _val = (val_expr);                                        \
    size_t _b = _ht->hash(&_key, _ht->key_size) % _ht->bucket_count;           \
    HNode **_buck = &_ht->buckets[_b];                                         \
    HNode *_n;                                                                 \
    int _found = 0;                                                            \
    for (_n = *_buck; _n; _n = _n->next) {                                     \
      if (_ht->eq(_n->data, &_key, _ht->key_size)) {                           \
        memcpy(_n->data + _ht->val_off, &_val, _ht->val_size);                 \
        _found = 1;                                                            \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (!_found) {                                                             \
      HNode *_new = malloc(sizeof(HNode) + _ht->val_off + _ht->val_size);      \
      assert(_new);                                                            \
      memcpy(_new->data, &_key, _ht->key_size);                                \
      memcpy(_new->data + _ht->val_off, &_val, _ht->val_size);                 \
      _new->next = *_buck;                                                     \
      *_buck = _new;                                                           \
      _ht->count++;                                                            \
    }                                                                          \
  } while (0)

// —— lookup: returns pointer to value or NULL ——
#define map_get(m, key_expr)                                                   \
  ({                                                                           \
    HashMap *_ht = (m).h;                                                      \
    typeof((m)._key) _key = (key_expr);                                        \
    size_t _b = _ht->hash(&_key, _ht->key_size) % _ht->bucket_count;           \
    HNode *_n = _ht->buckets[_b];                                              \
    typeof((m)._val) *_out = NULL;                                             \
    while (_n) {                                                               \
      if (_ht->eq(_n->data, &_key, _ht->key_size)) {                           \
        _out = (typeof((m)._val) *)(_n->data + _ht->val_off);                  \
        break;                                                                 \
      }                                                                        \
      _n = _n->next;                                                           \
    }                                                                          \
    _out;                                                                      \
  })

// —— remove a key: returns 1 if it was present, 0 otherwise ——
#define map_remove(m, key_expr)                                                \
  ({                                                                           \
    HashMap *_ht = (m).h;                                                      \
    typeof((m)._key) _key = (key_expr);                                        \
    size_t _b = _ht->hash(&_key, _ht->key_size) % _ht->bucket_count;           \
    HNode **_pp = &_ht->buckets[_b];                                           \
    int _removed = 0;                                                          \
    while (*_pp) {                                                             \
      HNode *_n = *_pp;                                                        \
      if (_ht->eq(_n->data, &_key, _ht->key_size)) {                           \
        *_pp = _n->next;                                                       \
        free(_n);                                                              \
        _ht->count--;                                                          \
        _removed = 1;                                                          \
        break;                                                                 \
      }                                                                        \
      _pp = &_n->next;                                                         \
    }                                                                          \
    _removed;                                                                  \
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
                     *(typeof((m)._val) *)(_n->data + (m).h->val_off);         \
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
                 *(typeof((m)._val) *)(_n->data + (m).h->val_off);             \
             _once_v; _once_v = 0)

#endif // HASHMAP_H
