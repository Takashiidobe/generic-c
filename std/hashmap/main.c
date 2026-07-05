#include "hashmap.h"

#include <stdio.h>
#include <string.h>

typedef struct {
  const char *value;
} Foo;

int main(void) {
  Map(int, Foo) map;
  map_init(map, 8);

  map_put(map, 1, (Foo){.value = "hi"});
  map_put(map, 2, (Foo){.value = "value"});
  map_put(map, 3, (Foo){.value = "next"});
  map_put(map, 2, (Foo){.value = "other"});

  assert(strcmp(map_get(map, 2)->value, "other") == 0);
  assert(map_get(map, 4) == NULL);
  assert(map_len(map) == 3);
  assert(map_remove(map, 2));
  assert(!map_remove(map, 2));
  assert(map_len(map) == 2);

  size_t pairs = 0;
  map_for(map, key, value) {
    assert((key == 1 && strcmp(value.value, "hi") == 0) ||
           (key == 3 && strcmp(value.value, "next") == 0));
    ++pairs;
  }
  assert(pairs == 2);

  size_t keys = 0;
  map_keys(map, key) {
    assert(key == 1 || key == 3);
    ++keys;
  }
  assert(keys == 2);

  size_t values = 0;
  map_vals(map, value) {
    assert(strcmp(value.value, "hi") == 0 ||
           strcmp(value.value, "next") == 0);
    ++values;
  }
  assert(values == 2);
  map_free(map);

  Map(const char *, int) strings;
  map_init_with(strings, 2, gc_str_hash, gc_str_eq);
  char string_keys[8][8];
  for (int i = 0; i < 8; ++i) {
    snprintf(string_keys[i], sizeof string_keys[i], "k%d", i);
    map_put(strings, string_keys[i], i * 10);
  }
  assert(map_len(strings) == 8);
  for (int i = 0; i < 8; ++i) {
    int *value = map_get(strings, string_keys[i]);
    assert(value && *value == i * 10);
  }
  map_free(strings);
}
