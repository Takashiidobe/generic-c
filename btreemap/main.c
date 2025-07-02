// test_sorted_map.c
#include "btreemap.h"
#include <stdio.h>

typedef struct {
  int value;
} Foo;

int main(void) {
  SortedMap(int, Foo) m;
  sorted_map_init(m, 4);

  sorted_map_put(m, 3, (Foo){.value = 30});
  sorted_map_put(m, 1, (Foo){.value = 10});
  sorted_map_put(m, 2, (Foo){.value = 20});
  sorted_map_put(m, 2, (Foo){.value = 200});
  sorted_map_put(m, 4, (Foo){.value = 44});

  Foo *p = sorted_map_get(m, 2);
  if (p)
    printf("Key 2 => %d\n", p->value);

  printf("All entries:\n");
  sorted_map_for(m, k, v) { printf("  %d => %d\n", k, v.value); }

  sorted_map_free(m);
  return 0;
}
