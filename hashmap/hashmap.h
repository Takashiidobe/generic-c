// hashmap.h
#ifndef HASHMAP_H
#define HASHMAP_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* —— internal node type (chaining) —— */
typedef struct HNode {
  struct HNode *next;
  char data[]; /* key bytes followed by value bytes */
} HNode;

/* —— the map “object” —— */
typedef struct {
  size_t key_size, val_size, bucket_count;
  HNode **buckets;
} HashMap;

/* —— alias for your map variable —— */
#define Map(K, V) HashMap *

/* —— initialize a Map(K,V) with `n_buckets` —— */
#define map_init(m, K, V, n_buckets)                                           \
  do {                                                                         \
    Map(K, V) _tmp = malloc(sizeof(HashMap));                                  \
    assert(_tmp);                                                              \
    _tmp->key_size = sizeof(K);                                                \
    _tmp->val_size = sizeof(V);                                                \
    _tmp->bucket_count = (n_buckets);                                          \
    _tmp->buckets = calloc(_tmp->bucket_count, sizeof(HNode *));               \
    assert(_tmp->buckets);                                                     \
    (m) = _tmp;                                                                \
  } while (0)

/* —— free everything — no type parameters needed —— */
#define map_free(m)                                                            \
  do {                                                                         \
    HashMap *_m = (m);                                                         \
    for (size_t _i = 0; _i < _m->bucket_count; ++_i) {                         \
      HNode *_n = _m->buckets[_i];                                             \
      while (_n) {                                                             \
        HNode *_nx = _n->next;                                                 \
        free(_n);                                                              \
        _n = _nx;                                                              \
      }                                                                        \
    }                                                                          \
    free(_m->buckets);                                                         \
    free(_m);                                                                  \
  } while (0)

/* —— a simple FNV-1a over raw bytes —— */
static inline size_t _hash_bytes(const void *data, size_t len) {
  size_t hash = (size_t)2166136261u;
  const unsigned char *p = data;
  for (size_t i = 0; i < len; i++) {
    hash ^= p[i];
    hash *= 16777619u;
  }
  return hash;
}

/* —— insert or overwrite —— */
#define map_put(m, K, V, key, val)                                             \
  do {                                                                         \
    Map(K, V) _m = (m);                                                        \
    /* stash into locals so we can take their address */                       \
    K _key = (key);                                                            \
    V _val = (val);                                                            \
    size_t _b = _hash_bytes(&_key, _m->key_size) % _m->bucket_count;           \
    HNode **_buck = &_m->buckets[_b];                                          \
    HNode *_n;                                                                 \
    int _found = 0;                                                            \
    /* scan for existing key */                                                \
    for (_n = *_buck; _n; _n = _n->next) {                                     \
      if (memcmp(_n->data, &_key, _m->key_size) == 0) {                        \
        /* overwrite value portion */                                          \
        memcpy(_n->data + _m->key_size, &_val, _m->val_size);                  \
        _found = 1;                                                            \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (!_found) {                                                             \
      /* not found → prepend new node */                                       \
      HNode *_new = malloc(sizeof(HNode) + _m->key_size + _m->val_size);       \
      assert(_new);                                                            \
      memcpy(_new->data, &_key, _m->key_size);                                 \
      memcpy(_new->data + _m->key_size, &_val, _m->val_size);                  \
      _new->next = *_buck;                                                     \
      *_buck = _new;                                                           \
    }                                                                          \
  } while (0)

/* —— lookup: returns V* or NULL —— */
#define map_get(m, K, V, key)                                                  \
  ({                                                                           \
    Map(K, V) _m = (m);                                                        \
    K _key = (key);                                                            \
    size_t _b = _hash_bytes(&_key, _m->key_size) % _m->bucket_count;           \
    HNode *_n = _m->buckets[_b];                                               \
    V *_out = NULL;                                                            \
    while (_n) {                                                               \
      if (memcmp(_n->data, &_key, _m->key_size) == 0) {                        \
        _out = (V *)(_n->data + _m->key_size);                                 \
        break;                                                                 \
      }                                                                        \
      _n = _n->next;                                                           \
    }                                                                          \
    _out;                                                                      \
  })

#endif // HASHMAP_H
