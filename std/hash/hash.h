#ifndef HASH_H
#define HASH_H
#include <stdlib.h>
#include <string.h>

// —— FNV-1a hash over raw bytes ——
static inline size_t gc_hash_bytes(const void *data, size_t len) {
  size_t hash = (size_t)2166136261u;
  const unsigned char *p = data;
  for (size_t i = 0; i < len; i++) {
    hash ^= p[i];
    hash *= 16777619u;
  }
  return hash;
}

// —— pluggable key hash / equality / ordering ——
// each callback receives a pointer to the stored key bytes plus the key size.
// for pointer keys (e.g. `const char *`) that pointer is a pointer-to-pointer,
// so the gc_str_* variants dereference once to reach the string contents.
typedef size_t (*gc_hash_fn)(const void *key, size_t key_size);
typedef int (*gc_eq_fn)(const void *a, const void *b, size_t key_size);
typedef int (*gc_cmp_fn)(const void *a, const void *b, size_t key_size);

// default: hash / compare the raw key bytes (correct for scalar keys)
static inline size_t gc_bytes_hash(const void *key, size_t key_size) {
  return gc_hash_bytes(key, key_size);
}
static inline int gc_bytes_eq(const void *a, const void *b, size_t key_size) {
  return memcmp(a, b, key_size) == 0;
}

// for `const char *` keys: follow the pointer and use the string contents
static inline size_t gc_str_hash(const void *key, size_t key_size) {
  (void)key_size;
  const char *s = *(const char *const *)key;
  return gc_hash_bytes(s, strlen(s));
}
static inline int gc_str_eq(const void *a, const void *b, size_t key_size) {
  (void)key_size;
  return strcmp(*(const char *const *)a, *(const char *const *)b) == 0;
}
static inline int gc_str_cmp(const void *a, const void *b, size_t key_size) {
  (void)key_size;
  return strcmp(*(const char *const *)a, *(const char *const *)b);
}

#endif
