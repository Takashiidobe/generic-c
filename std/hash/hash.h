#ifndef HASH_H
#define HASH_H
#include <stdlib.h>

// —— FNV-1a hash over raw bytes ——
size_t _hash_bytes(const void *data, size_t len) {
  size_t hash = (size_t)2166136261u;
  const unsigned char *p = data;
  for (size_t i = 0; i < len; i++) {
    hash ^= p[i];
    hash *= 16777619u;
  }
  return hash;
}

#endif
