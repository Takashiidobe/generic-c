// main.c
#include "hashmap.h"
#include <stdio.h>

typedef struct {
  int value;
} Foo;

int main(void) {
  /* declare and init a Map<int,Foo> with 16 buckets */
  Map(int, Foo) m;
  map_init(m, int, Foo, 16);

  /* insert two entries */
  map_put(m, int, Foo, 1, (Foo){.value = 10});
  map_put(m, int, Foo, 42, (Foo){.value = 99});

  /* lookup key=42 */
  Foo *f = map_get(m, int, Foo, 42);
  if (f)
    printf("got key 42 → %d\n", f->value);

  /* cleanup */
  map_free(m);
  return 0;
}
