#ifndef VECTOR_H
#define VECTOR_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*–– internal header stored *before* the user’s data ––*/
typedef struct {
  size_t length;
  size_t capacity;
  max_align_t data[];
} VecHeader;

/*–– the “handle” type for your vector ––*/
#define Vec(type) type *

/*–– get pointer to header from the data pointer ––*/
#define gc_vec_header(a)                                                       \
  ((VecHeader *)(void *)((char *)(a) - offsetof(VecHeader, data)))

/*–– length and capacity ––*/
#define vec_len(a) ((a) ? gc_vec_header(a)->length : 0)
#define vec_cap(a) ((a) ? gc_vec_header(a)->capacity : 0)

/*–– free the array ––*/
#define vec_free(a)                                                            \
  do {                                                                         \
    if (a) {                                                                   \
      free(gc_vec_header(a));                                                    \
      (a) = NULL;                                                              \
    }                                                                          \
  } while (0)

#define vec_reserve(a, n)                                                      \
  do {                                                                         \
    static_assert(_Alignof(typeof(*(a))) <= _Alignof(max_align_t),              \
                  "vector element alignment exceeds max_align_t");             \
    size_t _old_len = vec_len(a);                                              \
    if ((n) > vec_cap(a)) {                                                    \
      /* grow capacity up to at least n, but gc_vec_grow will also add to length \
       */                                                                      \
      gc_vec_grow((void **)&(a), (n) - _old_len, sizeof *(a));                   \
      /* undo the accidental length bump */                                    \
      gc_vec_header(a)->length = _old_len;                                       \
    }                                                                          \
  } while (0)

/*–– append one element ––*/
#define vec_push(a, val)                                                       \
  do {                                                                         \
    static_assert(_Alignof(typeof(*(a))) <= _Alignof(max_align_t),              \
                  "vector element alignment exceeds max_align_t");             \
    /* remember old length */                                                  \
    size_t _old_len = vec_len(a);                                              \
    /* grow by exactly one slot (this bumps length by +1) */                   \
    gc_vec_grow((void **)&(a), 1, sizeof *(a));                                  \
    /* store into the newly-allocated slot */                                  \
    (a)[_old_len] = (val);                                                     \
  } while (0)

/*–– remove & return last element ––*/
#define vec_pop(a) (assert(vec_len(a) > 0), (a)[--gc_vec_header(a)->length])

/*–– get the element at index `idx` (with bounds‐check) ––*/
#define vec_get(a, idx)                                                        \
  ({                                                                           \
    size_t gc_i = (idx);                                                        \
    /* must have been initialized and in‐bounds */                             \
    assert((a) && gc_i < vec_len(a));                                           \
    (a)[gc_i];                                                                  \
  })

/*–– set length to 0 (does not free memory) ––*/
#define vec_clear(a)                                                           \
  do {                                                                         \
    if (a)                                                                     \
      gc_vec_header(a)->length = 0;                                              \
  } while (0)

/*–– simple foreach ––*/
#define vec_for(item, a)                                                       \
  for (size_t _i = 0, _n = vec_len(a); _i < _n; ++_i)                          \
    for (__typeof__((a)[0]) *item = &(a)[_i]; item; item = NULL)

#define vec_enum(idx, item, a)                                                 \
  for (size_t idx = 0, _vec_enum_n = vec_len(a); idx < _vec_enum_n; ++idx)     \
    for (__typeof__((a)[0]) *item = &(a)[idx]; item; item = NULL)

/*–– internal grow routine ––*/
static inline void gc_vec_grow(void **arr, size_t increment, size_t elem_size) {
  VecHeader *h = *arr ? gc_vec_header(*arr) : NULL;
  size_t new_len = (h ? h->length : 0) + increment;
  size_t new_cap = h ? h->capacity : 0;

  if (new_len > new_cap) {
    /* geometric growth: double, or just big enough for new_len */
    size_t dbl = new_cap ? new_cap * 2 : 1;
    new_cap = new_len > dbl ? new_len : dbl;

    size_t header_size = offsetof(VecHeader, data);
    if (h) {
      h = realloc(h, header_size + new_cap * elem_size);
      assert(h);
    } else {
      h = malloc(header_size + new_cap * elem_size);
      assert(h);
      h->length = 0;
    }
    h->capacity = new_cap;
  }

  h->length += increment;
  *arr = h->data;
}

#define vec_init(a, ...)                                                       \
  do {                                                                         \
    static_assert(_Alignof(typeof(*(a))) <= _Alignof(max_align_t),              \
                  "vector element alignment exceeds max_align_t");             \
    /* count from sizeof on the array literal itself (unevaluated, no decay);  \
       _vec_src decays to a pointer only for the memcpy source */              \
    size_t _vec_n = sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0]);            \
    __auto_type _vec_src = (__VA_ARGS__);                                      \
    size_t _vec_hdr = offsetof(VecHeader, data);                               \
    size_t _vec_sz = _vec_hdr + _vec_n * sizeof _vec_src[0];                   \
    VecHeader *_vec_h = malloc(_vec_sz);                                       \
    assert(_vec_h);                                                            \
    _vec_h->length = _vec_n;                                                   \
    _vec_h->capacity = _vec_n;                                                 \
    memcpy(_vec_h->data, _vec_src, _vec_n * sizeof _vec_src[0]);               \
    (a) = (typeof(a))_vec_h->data;                                             \
  } while (0)

#endif // VECTOR_H
