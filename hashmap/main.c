#include "hashmap.h"
#include <stdio.h>

typedef struct {
  const char *value;
} Foo;

typedef struct {
  int value;
} Bar;

int main(void) {
  Map(int, Foo) m;
  map_init(m, 8);

  map_put(m, 1, (Foo){.value = "hi"});
  map_put(m, 2, (Foo){.value = "value"});
  map_put(m, 3, (Foo){.value = "next"});
  map_put(m, 2, (Foo){.value = "other"});
  // compiler error:
  // map_put(m, 2, (Bar){.value = 200});

  Foo *f2 = map_get(m, 2);
  if (f2)
    printf("Key 2 -> %s\n", f2->value);
  if (!map_get(m, 4))
    printf("Key 4 not found\n");

  printf("All (key,value) pairs:\n");
  map_for(m, k, v) { printf("  %d -> %s\n", k, v.value); }

  printf("Keys:\n");
  map_keys(m, k) { printf("  %d\n", k); }

  printf("Values:\n");
  map_vals(m, val) { printf("  %s\n", val.value); }

  map_free(m);
  return 0;
}
