#include "vector.h"

typedef struct {
  int value;
} Foo;

int main(void) {
  Vec(Foo) vec = NULL;
  vec_reserve(vec, 3);
  assert(vec_len(vec) == 0);
  assert(vec_cap(vec) >= 3);

  vec_push(vec, (Foo){.value = 3});
  vec_push(vec, (Foo){.value = 7});
  vec_push(vec, (Foo){.value = 42});
  assert(vec_len(vec) == 3);

  const int expected[] = {3, 7, 42};
  vec_enum(i, item, vec) { assert(item->value == expected[i]); }

  Foo last = vec_pop(vec);
  assert(last.value == 42);
  assert(vec_len(vec) == 2);

  vec_clear(vec);
  assert(vec_len(vec) == 0);
  vec_free(vec);
  assert(vec == NULL);
}
