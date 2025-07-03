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

  printf("len = %zu\n", map_len(m));
  printf("remove 2 -> %d\n", map_remove(m, 2));
  printf("remove 2 again -> %d\n", map_remove(m, 2));
  printf("len = %zu\n", map_len(m));

  printf("All (key,value) pairs:\n");
  map_for(m, k, v) { printf("  %d -> %s\n", k, v.value); }

  printf("Keys:\n");
  map_keys(m, k) { printf("  %d\n", k); }

  printf("Values:\n");
  map_vals(m, val) { printf("  %s\n", val.value); }

  map_free(m);

  // string keys compared by content, plus growth past the initial bucket count
  Map(const char *, int) s;
  map_init_with(s, 2, gc_str_hash, gc_str_eq);
  for (int i = 0; i < 8; ++i) {
    char *key = malloc(8);
    snprintf(key, 8, "k%d", i);
    map_put(s, key, i * 10);
  }
  int *v3 = map_get(s, "k3");
  printf("k3 -> %d, len = %zu\n", v3 ? *v3 : -1, map_len(s));
  map_keys(s, k) { free((void *)k); }
  map_free(s);
  return 0;
}
