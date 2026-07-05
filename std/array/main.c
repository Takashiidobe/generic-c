#include "vector.h"
#include "../test/assert.h"

typedef struct {
  int value;
} Foo;

int main(void) {
  Vec(Foo) vec = NULL;
  vec_reserve(vec, 3);
  GC_ASSERT(vec_len(vec) == 0);
  GC_ASSERT(vec_cap(vec) >= 3);

  vec_push(vec, (Foo){.value = 3});
  vec_push(vec, (Foo){.value = 7});
  vec_push(vec, (Foo){.value = 42});
  GC_ASSERT(vec_len(vec) == 3);

  const int expected[] = {3, 7, 42};
  vec_enum(i, item, vec) { GC_ASSERT(item->value == expected[i]); }

  Foo last = vec_pop(vec);
  GC_ASSERT(last.value == 42);
  GC_ASSERT(vec_len(vec) == 2);

  vec_clear(vec);
  GC_ASSERT(vec_len(vec) == 0);
  vec_free(vec);
  GC_ASSERT(vec == NULL);
}
