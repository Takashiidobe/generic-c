// test_bmap.c
#include "btreemap.h"
#include <stdio.h>

typedef struct {
  int value;
} Foo;

typedef struct {
  const char *value;
} Bar;

int main(void) {
  BMap(int, Foo) m;
  bmap_init(m, 4);

  bmap_put(m, 3, (Foo){.value = 30});
  bmap_put(m, 1, (Foo){.value = 10});
  bmap_put(m, 2, (Foo){.value = 20});
  bmap_put(m, 2, (Foo){.value = 200});
  bmap_put(m, 4, (Foo){.value = 44});

  Foo *p = bmap_get(m, 2);
  if (p)
    printf("Key 2 => %d\n", p->value);

  printf("All entries:\n");
  bmap_for(m, k, v) { printf("  %d => %d\n", k, v.value); }

  bmap_free(m);

  BMap(const char *, Bar) map;
  bmap_init(map, 1);
  bmap_put(map, "k", (Bar){.value = "xd"});
  bmap_put(map, "k2", (Bar){.value = "xdd"});
  bmap_put(map, "k1", (Bar){.value = "xddd"});

  printf("All entries:\n");
  bmap_for(map, k, v) { printf("  %s => %s\n", k, v.value); }

  return 0;
}
