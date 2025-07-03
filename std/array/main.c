#include "vector.h"
#include <stdio.h>

typedef struct {
  int value;
} Foo;

int main(void) {
  /* 1. declare a vector of Foo */
  Vec(Foo) foo_vec = NULL;

  /* 2. reserve more capacity if you like */
  vec_reserve(foo_vec, 3);

  /* 3. push some elements */
  vec_push(foo_vec, (Foo){.value = 3});
  vec_push(foo_vec, (Foo){.value = 7});
  vec_push(foo_vec, (Foo){.value = 42});

  /* 4. iterate */
  vec_for(item, foo_vec) { printf("foo.value = %d\n", item->value); }

  /* 5. pop the last element */
  Foo last = vec_pop(foo_vec);
  printf("popped: %d\n", last.value);

  /* 6. cleanup */
  vec_free(foo_vec);
  return 0;
}
