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
  char data[]; /* flexible array member */
} VecHeader;

/*–– the “handle” type for your vector ––*/
#define Vec(type) type *

/*–– get pointer to header from the data pointer ––*/
#define _vec_header(a) ((VecHeader *)((char *)(a) - offsetof(VecHeader, data)))

/*–– length and capacity ––*/
#define vec_len(a) ((a) ? _vec_header(a)->length : 0)
#define vec_cap(a) ((a) ? _vec_header(a)->capacity : 0)

/*–– free the array ––*/
#define vec_free(a)                                                            \
  do {                                                                         \
    if (a) {                                                                   \
      free(_vec_header(a));                                                    \
      (a) = NULL;                                                              \
    }                                                                          \
  } while (0)

/*–– ensure room for at least `n` elements ––*/
#define vec_reserve(a, n)                                                      \
  _vec_grow((void **)&(a), (n) - vec_len(a), sizeof *(a))

/*–– append one element ––*/
#define vec_push(a, val)                                                       \
  do {                                                                         \
    _vec_grow((void **)&(a), 1, sizeof *(a));                                  \
    (a)[_vec_header(a)->length - 1] = (val);                                   \
  } while (0)

/*–– remove & return last element ––*/
#define vec_pop(a) ((a)[--_vec_header(a)->length])

/*–– set length to 0 (does not free memory) ––*/
#define vec_clear(a)                                                           \
  do {                                                                         \
    if (a)                                                                     \
      _vec_header(a)->length = 0;                                              \
  } while (0)

/*–– simple foreach ––*/
#define vec_for(item, a)                                                       \
  for (size_t _i = 0, _n = vec_len(a); _i < _n; ++_i)                          \
    for (__typeof__((a)[0]) *item = &(a)[_i]; item; item = NULL)

/*–– internal grow routine ––*/
static void _vec_grow(void **arr, size_t increment, size_t elem_size) {
  VecHeader *h = *arr ? _vec_header(*arr) : NULL;
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

#endif // VECTOR_H
